/* $Id: libk8055.c,v 1.7 2008/08/20 17:00:55 mr_brain Exp $

   This file is part of the libk8055 Library.

   The libk8055 Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The libk8055 Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the
   Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   http://opensource.org/licenses/

   Copyleft (C) 2005 by Sven Lindberg
     k8055@k8055.mine.nu

   Copyright (C) 2007 by Pjetur G. Hjaltason
       pjetur@pjetur.net
       Commenting, general rearrangement of code, bugfixes,
       python interface with swig and simple k8055 python class


	Input packet format

	+---+---+---+---+---+---+---+---+
	|DIn|Sta|A1 |A2 |   C1  |   C2  |
	+---+---+---+---+---+---+---+---+
	DIn = Digital input in high nibble, except for input 3 in 0x01
	Sta = Status, Board number + 1
	A1  = Analog input 1, 0-255
	A2  = Analog input 2, 0-255
	C1  = Counter 1, 16 bits (lsb)
	C2  = Counter 2, 16 bits (lsb)

	Output packet format

	+---+---+---+---+---+---+---+---+
	|CMD|DIG|An1|An2|Rs1|Rs2|Dbv|Dbv|
	+---+---+---+---+---+---+---+---+
	CMD = Command
	DIG = Digital output bitmask
	An1 = Analog output 1 value, 0-255
	An2 = Analog output 2 value, 0-255
	Rs1 = Reset counter 1, command 3
	Rs2 = Reset counter 3, command 4
	Dbv = Debounce value for counter 1 and 2, command 1 and 2

	Or split by commands

	Cmd 0, Reset ??
	Cmd 1, Set debounce Counter 1
	+---+---+---+---+---+---+---+---+
	|CMD|   |   |   |   |   |Dbv|   |
	+---+---+---+---+---+---+---+---+
	Cmd 2, Set debounce Counter 2
	+---+---+---+---+---+---+---+---+
	|CMD|   |   |   |   |   |   |Dbv|
	+---+---+---+---+---+---+---+---+
	Cmd 3, Reset counter 1
	+---+---+---+---+---+---+---+---+
	| 3 |   |   |   | 00|   |   |   |
	+---+---+---+---+---+---+---+---+
	Cmd 4, Reset counter 2
	+---+---+---+---+---+---+---+---+
	| 4 |   |   |   |   | 00|   |   |
	+---+---+---+---+---+---+---+---+
	cmd 5, Set analog/digital
	+---+---+---+---+---+---+---+---+
	| 5 |DIG|An1|An2|   |   |   |   |
	+---+---+---+---+---+---+---+---+

**/

#include <string.h>
#include <stdio.h>
#include <usb.h>
#include <assert.h>
#include <math.h>
#include "k8055.h"

#define STR_BUFF 256
#define PACKET_LEN 8
#define READ_RETRY 3
#define WRITE_RETRY 3

#define K8055_IPID 0x5500
#define VELLEMAN_VENDOR_ID 0x10cf
#define K8055_MAX_DEV 4

#define USB_OUT_EP 0x01	/* USB output endpoint */
#define USB_INP_EP 0x81 /* USB Input endpoint */

#define USB_TIMEOUT 20
#define K8055_ERROR -1

#define DIGITAL_INP_OFFSET 0
#define DIGITAL_OUT_OFFSET 1
#define ANALOG_1_OFFSET 2
#define ANALOG_2_OFFSET 3
#define COUNTER_1_OFFSET 4
#define COUNTER_2_OFFSET 6

#define CMD_RESET 0x00
#define CMD_SET_DEBOUNCE_1 0x01
#define CMD_SET_DEBOUNCE_2 0x01
#define CMD_RESET_COUNTER_1 0x03
#define CMD_RESET_COUNTER_2 0x04
#define CMD_SET_ANALOG_DIGITAL 0x05

/* set debug to 0 to not print excess info */
int debug = 0;

/* globals for datatransfer */
struct k8055_dev {
    unsigned char data_in[PACKET_LEN+1];
    unsigned char data_out[PACKET_LEN+1];
    struct usb_dev_handle *device_handle;
    int dev_no;
};

static struct k8055_dev k8055d[K8055_MAX_DEV];
static struct k8055_dev *curr_dev;

/* Actual read of data from the device endpoint, retry READ_RETRY times if not responding ok */
static int k8055_read( struct k8055_dev* dev ) {
    if(dev->dev_no==0) return K8055_ERROR;
    for(int i=0; i<READ_RETRY; i++) {
        int read_status = usb_interrupt_read(dev->device_handle, USB_INP_EP, (char*)dev->data_in, PACKET_LEN, USB_TIMEOUT);
        if( (read_status==PACKET_LEN) && (dev->data_in[1]==dev->dev_no) ) {
            if(debug) fprintf(stderr,"read dev %d data : %X\n",dev->data_in[1],dev->data_in);
            return 0;
        }
        if(debug) fprintf(stderr, "k8055 read retry\n");
    }
    return K8055_ERROR;
}

/* Actual write of data to the device endpont, retry WRITE_RETRY times if not reponding correctly */
static int k8055_write( struct k8055_dev* dev ) {
    if(dev->dev_no == 0) return K8055_ERROR;
    for(int i=0; i<WRITE_RETRY; i++) {
        int write_status = usb_interrupt_write(dev->device_handle, USB_OUT_EP, (char*)dev->data_out, PACKET_LEN, USB_TIMEOUT);
        if(write_status==PACKET_LEN) return 0;
        if(debug) fprintf(stderr, "k8055 write retry\n");
    }
    return K8055_ERROR;
}

/* If device is owned by some kernel driver, try to disconnect it and claim the device*/
static int takeover_device(usb_dev_handle * udev, int interface) {
    char driver_name[STR_BUFF];
    memset(driver_name, 0, STR_BUFF);
    int ret = K8055_ERROR;
    assert(udev != NULL);
    if(usb_get_driver_np(udev, interface, driver_name, sizeof(driver_name))==0) {
        if(debug) fprintf(stderr, "usb_get_driver_np success: %s\n", driver_name);
        if(usb_detach_kernel_driver_np(udev, interface)==0) {
            if(debug) fprintf(stderr, "usb_detach_kernel_driver_np success");
        } else {
            if(debug) fprintf(stderr, "usb_detach_kernel_driver_np failure : %s\n", usb_strerror());
        }
    } else {
        if(debug) fprintf(stderr, "usb_get_driver_np failure : %s\n", usb_strerror());
    }
    if (usb_claim_interface(udev, interface)==0) {
        usb_set_altinterface(udev, interface);
    } else {
        if(debug) fprintf(stderr, "usb_claim_interface failure: %s\n", usb_strerror());
        return K8055_ERROR;
    }
    usb_set_configuration(udev, 1);
    if (debug) fprintf(stderr, "Found interface %d, took over the device\n", interface);
    return 0;
}

/* Open device - scan through usb busses looking for the right device, claim it and then open the device */
int OpenDevice( long board_address ) {
    if( board_address<0 || board_address>=K8055_MAX_DEV ) return K8055_ERROR;
    if(k8055d[board_address].dev_no!=0) return board_address;
    usb_init();
    usb_find_busses();
    usb_find_devices();
    int ipid = K8055_IPID + (int)board_address;
    struct usb_bus* busses = usb_get_busses();
    for( struct usb_bus* bus=busses; bus; bus=bus->next ) {
        for( struct usb_device* dev=bus->devices; dev; dev=dev->next ) {
            if((dev->descriptor.idVendor==VELLEMAN_VENDOR_ID) && (dev->descriptor.idProduct==ipid)) {
                struct k8055_dev *kdev = &k8055d[board_address];
                kdev->dev_no = 0;
                kdev->device_handle = usb_open(dev);
                if(kdev->device_handle==0) {
                    if(debug) fprintf(stderr,"usb_open failure : %s\n", usb_strerror());
                    return K8055_ERROR;
                }
                if(debug) fprintf(stderr, "Velleman Device Found @ Address %s Vendor 0x0%x Product ID 0x0%x\n", dev->filename, dev->descriptor.idVendor, dev->descriptor.idProduct);
                if(takeover_device(kdev->device_handle, 0)<0) {
                    if(debug) fprintf(stderr, "Can not take over the device from the OS driver\n");
                    usb_close(kdev->device_handle);
                    kdev->device_handle = NULL;
                    return K8055_ERROR;
                } else {
                    memset(kdev->data_out,0,PACKET_LEN);
                    kdev->dev_no = board_address + 1;
                    kdev->data_out[0] = CMD_RESET;
                    k8055_write(kdev);
                    if (k8055_read(kdev)==0) {
                        if(debug) fprintf(stderr, "Device %d ready\n",board_address);
                        curr_dev = kdev;
                        return board_address;
                    } else {
                        if(debug) fprintf(stderr, "Device %d not ready\n",board_address);
                        kdev->dev_no = 0;
                        usb_close(kdev->device_handle);
                        kdev->device_handle = NULL;
                        return K8055_ERROR;
                    }
                }
            }
        }
    }
    if(debug) fprintf(stderr, "Could not find Velleman k8055 with address %d\n",(int)board_address);
    return K8055_ERROR;
}

/* Close the Current device */
int CloseDevice() {
    if (curr_dev->dev_no == 0) {
        if (debug) fprintf(stderr, "Current device is not open\n" );
        return 0;
    }
    if(curr_dev->device_handle==NULL) {
        if (debug) fprintf(stderr, "Current device is marked as open, but device hanlde is NULL\n" );
        curr_dev->dev_no = 0;
        return 0;
    }
    int rc = usb_close(curr_dev->device_handle);
    if (rc >= 0) {
        curr_dev->dev_no = 0;
        curr_dev->device_handle = NULL;
    }
    return rc;
}

/* New function in version 2 of Velleman DLL, should return deviceno if OK */
long SetCurrentDevice(long deviceno) {
    if (deviceno < 0 || deviceno >= K8055_MAX_DEV) return K8055_ERROR;
    if (k8055d[deviceno].dev_no == 0) return K8055_ERROR;
    curr_dev  = &k8055d[deviceno];
    return deviceno;
}

/* New function in version 2 of Velleman DLL, should return devices-found bitmask or 0*/
long SearchDevices(void) {
    int retval = 0;
    usb_init();
    usb_find_busses();
    usb_find_devices();
    struct usb_bus* busses = usb_get_busses();
    for (struct usb_bus* bus = busses; bus; bus = bus->next) {
        for( struct usb_device* dev=bus->devices; dev; dev=dev->next ) {
            if (dev->descriptor.idVendor == VELLEMAN_VENDOR_ID) { 
                if(dev->descriptor.idProduct == K8055_IPID + 0) retval |= 0x01;
                if(dev->descriptor.idProduct == K8055_IPID + 1) retval |= 0x02;
                if(dev->descriptor.idProduct == K8055_IPID + 2) retval |= 0x04;
                if(dev->descriptor.idProduct == K8055_IPID + 3) retval |= 0x08;
                /* else some other kind of Velleman board */
            }
        }
    }
    if(debug) fprintf(stderr,"found devices : %X\n",retval);
    return retval; 
}

long ReadAnalogChannel(long channel) {
    if (!(channel==1 || channel==2)) return K8055_ERROR;
    if ( k8055_read(curr_dev)==0) {
        if (channel==1) {
            return curr_dev->data_in[ANALOG_1_OFFSET];
        } else {
            return curr_dev->data_in[ANALOG_2_OFFSET];
        }
    }
    return K8055_ERROR;
}

int ReadAllAnalog(long *data1, long *data2) {
    if ( k8055_read(curr_dev)!=0 ) return K8055_ERROR;
    *data1 = curr_dev->data_in[ANALOG_1_OFFSET];
    *data2 = curr_dev->data_in[ANALOG_2_OFFSET];
    return 0;
}

int OutputAnalogChannel(long channel, long data) {
    if (!(channel==1 || channel==2)) return K8055_ERROR;
    curr_dev->data_out[0] = CMD_SET_ANALOG_DIGITAL;
    if (channel==1) {
        curr_dev->data_out[ANALOG_1_OFFSET] = (unsigned char)data;
    } else {
        curr_dev->data_out[ANALOG_2_OFFSET] = (unsigned char)data;
    }
    return k8055_write(curr_dev);
}

int OutputAllAnalog(long data1, long data2) {
    curr_dev->data_out[0] = CMD_SET_ANALOG_DIGITAL;
    curr_dev->data_out[2] = (unsigned char)data1;
    curr_dev->data_out[3] = (unsigned char)data2;
    return k8055_write(curr_dev);
}

int ClearAllAnalog() {
    return OutputAllAnalog(0, 0);
}

int ClearAnalogChannel(long channel) {
    if (!(channel==1 || channel==2)) return K8055_ERROR;
    if (channel==1) {
        return OutputAnalogChannel(1, 0);
    } else {
        return OutputAnalogChannel(2, 0);
    }
}

int SetAnalogChannel(long channel) {
    if (!(channel==1 || channel==2)) return K8055_ERROR;
    if (channel == 2) {
        return OutputAnalogChannel(2, 0xff);
    } else {
        return OutputAnalogChannel(1, 0xff);
    }
}

int SetAllAnalog() {
    return OutputAllAnalog(0xff, 0xff);
}

int WriteAllDigital(long data) {
    curr_dev->data_out[0] = CMD_SET_ANALOG_DIGITAL;
    curr_dev->data_out[1] = (unsigned char)data;
    return k8055_write(curr_dev);
}

int ClearDigitalChannel(long channel) {
    unsigned char data;
    if (channel<1 || channel>8) return K8055_ERROR;
    data = curr_dev->data_out[1] & ~(1 << (channel-1));
    return WriteAllDigital(data);
}

int ClearAllDigital() {
    return WriteAllDigital(0x00);
}

int SetDigitalChannel(long channel) {
    unsigned char data;
    if (channel<1 || channel>8) return K8055_ERROR;
    data = curr_dev->data_out[1] | (1 << (channel-1));
    return WriteAllDigital(data);
}

int SetAllDigital() {
    return WriteAllDigital(0xff);
}

int ReadDigitalChannel(long channel) {
    int rval;
    if (channel<1 || channel>8) return K8055_ERROR;
    if ((rval = ReadAllDigital()) == K8055_ERROR) return K8055_ERROR;
    return ((rval & (1 << (channel-1))) > 0);
}

long ReadAllDigital() {
    int return_data = 0;
    if ( k8055_read(curr_dev)!=0) return K8055_ERROR;
    return_data = (
            ((curr_dev->data_in[0] >> 4) & 0x03) |  /* Input 1 and 2 */
            ((curr_dev->data_in[0] << 2) & 0x04) |  /* Input 3 */
            ((curr_dev->data_in[0] >> 3) & 0x18) ); /* Input 4 and 5 */
    return return_data;
}

int ReadAllValues(long int *data1, long int * data2, long int * data3, long int * data4, long int * data5) {
    if ( k8055_read(curr_dev)!=0) return K8055_ERROR;
    *data1 = (
            ((curr_dev->data_in[0] >> 4) & 0x03) |  /* Input 1 and 2 */
            ((curr_dev->data_in[0] << 2) & 0x04) |  /* Input 3 */
            ((curr_dev->data_in[0] >> 3) & 0x18) ); /* Input 4 and 5 */
    *data2 = curr_dev->data_in[ANALOG_1_OFFSET];
    *data3 = curr_dev->data_in[ANALOG_2_OFFSET];
    *data4 = *((short int *)(&curr_dev->data_in[COUNTER_1_OFFSET]));
    *data5 = *((short int *)(&curr_dev->data_in[COUNTER_2_OFFSET]));
    return 0;
}

int SetAllValues(int DigitalData, int AdData1, int AdData2) {
    curr_dev->data_out[0] = CMD_SET_ANALOG_DIGITAL;
    curr_dev->data_out[1] = (unsigned char)DigitalData;
    curr_dev->data_out[2] = (unsigned char)AdData1;
    curr_dev->data_out[3] = (unsigned char)AdData2;
    return k8055_write(curr_dev);
}

int ResetCounter(long counter) {
    if (!(counter==1 || counter==2)) return K8055_ERROR;
    curr_dev->data_out[0] = 0x02 + (unsigned char)counter;
    curr_dev->data_out[3+counter] = 0x00;
    return k8055_write(curr_dev);
}

long ReadCounter(long counter) {
    if (!(counter==1 || counter==2)) return K8055_ERROR;
    if (k8055_read(curr_dev)!=0) return K8055_ERROR;
    if (counter==1) {
        return *((short int *)(&curr_dev->data_in[COUNTER_1_OFFSET]));
    } else {
        return *((short int *)(&curr_dev->data_in[COUNTER_2_OFFSET]));
    }
}

int SetCounterDebounceTime(long counter, long debounce_time) {
    float value;
    if (!(counter==1 || counter==2)) return K8055_ERROR;
    curr_dev->data_out[0] = (unsigned char)counter;
    /*
     * the velleman k8055 use a exponetial formula to split up the
     * debounce_time 0-7450 over value 1-255. I've tested every value and
     * found that the formula dbt=0,338*value^1,8017 is closest to
     * vellemans dll. By testing and measuring times on the other hand I
     * found the formula dbt=0,115*x^2 quite near the actual values, a
     * little below at really low values and a little above at really
     * high values. But the time set with this formula is within +-4%
     *  -- Sven Lindberg
     */
    if (debounce_time > 7450) debounce_time = 7450;
    value = sqrtf(debounce_time / 0.115);
    /* simple round() function) */
    if (value > ((int)value + 0.49999999)) value+=1;
    curr_dev->data_out[5+counter] = (unsigned char)value;
    if (debug) fprintf(stderr, "Debouncetime%d value for k8055:%d\n",(int)counter, curr_dev->data_out[5+counter]);
    return k8055_write(curr_dev);
}

char * Version(void) {
    return(VERSION);
}
