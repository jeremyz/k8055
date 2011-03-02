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
#include <assert.h>
#include <math.h>
#include <libusb.h>
#include "k8055.h"

#define STR_BUFF 256
#define READ_RETRY 3
#define WRITE_RETRY 3

#define K8055_IPID 0x5500
#define VELLEMAN_VENDOR_ID 0x10cf

#define USB_OUT_EP 0x01	/* USB output endpoint */
#define USB_INP_EP 0x81 /* USB Input endpoint */

#define USB_TIMEOUT 20
#define K8055_ERROR -1

#define CMD_OFFSET 0
#define CMD_RESET 0x00
#define CMD_SET_DEBOUNCE_1 0x01
#define CMD_SET_DEBOUNCE_2 0x01
#define CMD_RESET_COUNTER_1 0x03
#define CMD_RESET_COUNTER_2 0x04
#define CMD_SET_ANALOG_DIGITAL 0x05

/* set debug to 0 to not print excess info */
static int debug = 0;

/* Actual read of data from the device endpoint, retry READ_RETRY times if not responding ok */
int k8055_read( struct k8055_dev* dev ) {
    if( dev->dev_no==0 ) return K8055_ERROR;
    int length;
    for( int i=0; i<READ_RETRY; i++ ) {
        if( libusb_interrupt_transfer( dev->usb_handle, USB_INP_EP, dev->data_in, PACKET_LEN, &length, USB_TIMEOUT )==0 && ( length==PACKET_LEN ) && ( dev->data_in[1]==dev->dev_no ) ) return 0;
        if( debug ) fprintf( stderr, "k8055 read retry\n" );
    }
    return K8055_ERROR;
}

/* Actual write of data to the device endpont, retry WRITE_RETRY times if not reponding correctly */
int k8055_write( struct k8055_dev* dev ) {
    if( dev->dev_no == 0 ) return K8055_ERROR;
    int length;
    for( int i=0; i<WRITE_RETRY; i++ ) {
        if( libusb_interrupt_transfer( dev->usb_handle, USB_OUT_EP, dev->data_out, PACKET_LEN, &length, USB_TIMEOUT )==0 && length==PACKET_LEN ) return 0;
        if( debug ) fprintf( stderr, "k8055 write retry\n" );
    }
    return K8055_ERROR;
}

int k8055_digital_1( struct k8055_dev* dev ) {
    return ( ( dev->data_in[DIGITAL_INP_OFFSET] >> 4 ) & 0x01 );
}

int k8055_digital_2( struct k8055_dev* dev ) {
    return ( ( dev->data_in[DIGITAL_INP_OFFSET] >> 5 ) & 0x01 );
}

int k8055_digital_3( struct k8055_dev* dev ) {
    return ( ( dev->data_in[DIGITAL_INP_OFFSET] ) & 0x01 );
}

int k8055_digital_4( struct k8055_dev* dev ) {
    return ( ( dev->data_in[DIGITAL_INP_OFFSET] >> 6 ) & 0x01 );
}

int k8055_digital_5( struct k8055_dev* dev ) {
    return ( ( dev->data_in[DIGITAL_INP_OFFSET] >> 7 ) & 0x01 );
}

int k8055_analog_1( struct k8055_dev* dev ) {
    return dev->data_in[ANALOG_1_OFFSET];
}

int k8055_analog_2( struct k8055_dev* dev ) {
    return dev->data_in[ANALOG_2_OFFSET];
}

int k8055_counter_1( struct k8055_dev* dev ) {
    return dev->data_in[COUNTER_1_OFFSET];
}

int k8055_counter_2( struct k8055_dev* dev ) {
    return dev->data_in[COUNTER_2_OFFSET];
}

/* If device is owned by some kernel driver, try to disconnect it and claim the device*/
static int k8055_takeover_device( libusb_device_handle* handle, int interface ) {
    char driver_name[STR_BUFF];
    memset( driver_name, 0, STR_BUFF );
    int ret = K8055_ERROR;
    assert( handle != NULL );
    if( libusb_detach_kernel_driver( handle, interface )!=0 ) {
        if( debug ) fprintf( stderr, "usb_detach_kernel_driver failure\n" );
    }
    if ( libusb_claim_interface( handle, interface )!=0 ) {
        if( debug ) fprintf( stderr, "usb_claim_interface failure\n" );
        return K8055_ERROR;
    }
    libusb_set_configuration( handle, 1 );
    if ( debug ) fprintf( stderr, "Found interface %d, took over the device\n", interface );
    return 0;
}

void k8055_set_debug_on( void ) {
    debug = 1;
}

void k8055_set_debug_off( void ) {
    debug = 0;
}

char* k8055_version( void ) {
    return( VERSION );
}

int k8055_open_device( struct k8055_dev* dev, int board_address ) {
    int ipid = K8055_IPID + board_address;
    libusb_device** list;
    libusb_device* found = NULL;
    libusb_init( &dev->usb_ctx );
    ssize_t cnt = libusb_get_device_list( NULL, &list );
    if( cnt<0 ) {
        if( debug ) fprintf( stderr, "Unable to list usb devices\n" );
        return K8055_ERROR;
    }
    for ( ssize_t i=0; i<cnt; i++ ) {
        libusb_device* usb_dev = list[i];
        struct libusb_device_descriptor usb_descr;
        if( libusb_get_device_descriptor( usb_dev, &usb_descr )==0 ) {
            if( ( usb_descr.idVendor==VELLEMAN_VENDOR_ID ) && ( usb_descr.idProduct==ipid ) ) {
                // TODO  was usb_dev->filename
                if( debug ) fprintf( stderr, "Velleman Device Found @ Address %s Vendor 0x0%x Product ID 0x0%x\n", "002", usb_descr.idVendor, usb_descr.idProduct );
                found = usb_dev;
                break;
            }
        } else {
            if( debug ) fprintf( stderr, "USB device descriptor unaccessible.\n" );
        }
    }
    if( found==NULL ) {
        if( debug ) fprintf( stderr, "No Velleman device found.\n" );
        return K8055_ERROR;
    }
    dev->dev_no = 0;
    dev->usb_handle = NULL;
    if( libusb_open( found , &dev->usb_handle )!=0 ) {
        if( debug ) fprintf( stderr,"usb_open failure\n" );
        return K8055_ERROR;
    }
    if( k8055_takeover_device( dev->usb_handle, 0 )!=0 ) {
        if( debug ) fprintf( stderr, "Can not take over the device from the OS driver\n" );
        libusb_release_interface( dev->usb_handle, 0 );
        libusb_close( dev->usb_handle );
        libusb_exit( dev->usb_ctx );
        dev->usb_handle = NULL;
        return K8055_ERROR;
    } else {
        memset( dev->data_out,0,PACKET_LEN );
        dev->dev_no = board_address + 1;
        dev->data_out[CMD_OFFSET] = CMD_RESET;
        k8055_write( dev );
        if ( k8055_read( dev )==0 ) {
            if( debug ) fprintf( stderr, "Device %d ready\n", board_address );
            return board_address;
        } else {
            if( debug ) fprintf( stderr, "Device %d not ready\n", board_address );
            libusb_release_interface( dev->usb_handle, 0 );
            libusb_close( dev->usb_handle );
            libusb_exit( dev->usb_ctx );
            dev->dev_no = 0;
            dev->usb_handle = NULL;
            return K8055_ERROR;
        }
    }
    if( debug ) fprintf( stderr, "Could not find Velleman k8055 with address %d\n", board_address );
    return K8055_ERROR;
}

int k8055_close_device( struct k8055_dev* dev ) {
    if ( dev->dev_no == 0 ) {
        if ( debug ) fprintf( stderr, "Current device is not open\n" );
    } else if( dev->usb_handle==NULL ) {
        if ( debug ) fprintf( stderr, "Current device is marked as open, but device hanlde is NULL\n" );
        dev->dev_no = 0;
    } else {
        if( libusb_release_interface( dev->usb_handle, 0 )!= 0 ) {
            if( debug ) fprintf( stderr,"libusb_realese_interface failure.\n" );
        }
        libusb_close( dev->usb_handle );
        dev->dev_no = 0;
        dev->usb_handle = NULL;
    }
    return 0;
}

int k8055_search_devices( void ) {
    int ret = 0;
    struct libusb_context* usb_ctx;
    libusb_device** list;
    struct libusb_device_descriptor usb_descr;
    libusb_init( &usb_ctx );
    ssize_t cnt = libusb_get_device_list( NULL, &list );
    for ( ssize_t i=0; i<cnt; i++ ) {
        if( libusb_get_device_descriptor( list[i], &usb_descr )==0 ) {
            if( usb_descr.idVendor==VELLEMAN_VENDOR_ID ) {
                if( usb_dev->descriptor.idProduct == K8055_IPID + 0 ) ret |= 0x01;
                if( usb_dev->descriptor.idProduct == K8055_IPID + 1 ) ret |= 0x02;
                if( usb_dev->descriptor.idProduct == K8055_IPID + 2 ) ret |= 0x04;
                if( usb_dev->descriptor.idProduct == K8055_IPID + 3 ) ret |= 0x08;
                /* else some other kind of Velleman board */
            }
        } else {
            if( debug ) fprintf( stderr, "USB device descriptor unaccessible.\n" );
        }
    }
    if( debug ) fprintf( stderr,"found devices : %X\n", ret );
    libusb_exit( usb_ctx );
    return ret;
}

int k8055_read_analog_channel( struct k8055_dev* dev, int channel ) {
    if ( !( channel==1 || channel==2 ) ) return K8055_ERROR;
    if ( k8055_read( dev )==0 ) {
        if ( channel==1 ) {
            return dev->data_in[ANALOG_1_OFFSET];
        } else {
            return dev->data_in[ANALOG_2_OFFSET];
        }
    }
    return K8055_ERROR;
}

int k8055_read_all_analog( struct k8055_dev* dev, int* data1, int* data2 ) {
    if ( k8055_read( dev )!=0 ) return K8055_ERROR;
    *data1 = dev->data_in[ANALOG_1_OFFSET];
    *data2 = dev->data_in[ANALOG_2_OFFSET];
    return 0;
}

int k8055_write_analog_channel( struct k8055_dev* dev ,int channel, int data ) {
    if ( !( channel==1 || channel==2 ) ) return K8055_ERROR;
    dev->data_out[CMD_OFFSET] = CMD_SET_ANALOG_DIGITAL;
    if ( channel==1 ) {
        dev->data_out[ANALOG_1_OFFSET] = ( unsigned char )data;
    } else {
        dev->data_out[ANALOG_2_OFFSET] = ( unsigned char )data;
    }
    return k8055_write( dev );
}

int k8055_write_all_analog( struct k8055_dev* dev, int data1, int data2 ) {
    dev->data_out[CMD_OFFSET] = CMD_SET_ANALOG_DIGITAL;
    dev->data_out[ANALOG_1_OFFSET] = ( unsigned char )data1;
    dev->data_out[ANALOG_2_OFFSET] = ( unsigned char )data2;
    return k8055_write( dev );
}

int k8055_clear_all_analog( struct k8055_dev* dev ) {
    return k8055_write_all_analog( dev, 0, 0 );
}

int k8055_clear_analog_channel( struct k8055_dev* dev, int channel ) {
    if ( !( channel==1 || channel==2 ) ) return K8055_ERROR;
    if ( channel==1 ) {
        return k8055_write_analog_channel( dev, 1, 0 );
    } else {
        return k8055_write_analog_channel( dev, 2, 0 );
    }
}

int k8055_set_analog_channel( struct k8055_dev* dev, int channel ) {
    if ( !( channel==1 || channel==2 ) ) return K8055_ERROR;
    if ( channel == 2 ) {
        return k8055_write_analog_channel( dev, 2, 0xff );
    } else {
        return k8055_write_analog_channel( dev, 1, 0xff );
    }
}

int k8055_set_all_analog( struct k8055_dev* dev ) {
    return k8055_write_all_analog( dev, 0xff, 0xff );
}

int k8055_write_all_digital( struct k8055_dev* dev, int data ) {
    dev->data_out[CMD_OFFSET] = CMD_SET_ANALOG_DIGITAL;
    dev->data_out[DIGITAL_OUT_OFFSET] = ( unsigned char )data;
    return k8055_write( dev );
}

int k8055_clear_digital_channel( struct k8055_dev* dev, int channel ) {
    unsigned char data;
    if ( channel<1 || channel>8 ) return K8055_ERROR;
    data = dev->data_out[DIGITAL_OUT_OFFSET] & ~( 1 << ( channel-1 ) );
    return k8055_write_all_digital( dev, data );
}

int k8055_clear_all_digital( struct k8055_dev* dev ) {
    return k8055_write_all_digital( dev, 0x00 );
}

int k8055_set_digital_channel( struct k8055_dev* dev, int channel ) {
    unsigned char data;
    if ( channel<1 || channel>8 ) return K8055_ERROR;
    data = dev->data_out[DIGITAL_OUT_OFFSET] | ( 1 << ( channel-1 ) );
    return k8055_write_all_digital( dev, data );
}

int k8055_set_all_digital( struct k8055_dev* dev ) {
    return k8055_write_all_digital( dev, 0xff );
}

int k8055_read_digital_channel( struct k8055_dev* dev, int channel ) {
    int rval;
    if ( channel<1 || channel>5 ) return K8055_ERROR;
    if ( ( rval = k8055_read_all_digital( dev ) ) == K8055_ERROR ) return K8055_ERROR;
    return ( ( rval & ( 1 << ( channel-1 ) ) ) > 0 );
}

int k8055_read_all_digital( struct k8055_dev* dev ) {
    int return_data = 0;
    if ( k8055_read( dev )!=0 ) return K8055_ERROR;
    return_data = (
                      ( ( dev->data_in[DIGITAL_INP_OFFSET] >> 4 ) & 0x03 ) | /* Input 1 and 2 */
                      ( ( dev->data_in[DIGITAL_INP_OFFSET] << 2 ) & 0x04 ) | /* Input 3 */
                      ( ( dev->data_in[DIGITAL_INP_OFFSET] >> 3 ) & 0x18 ) );/* Input 4 and 5 */
    return return_data;
}

int k8055_read_all_values( struct k8055_dev* dev, int* data1, int* data2, int* data3, int* data4, int* data5 ) {
    if ( k8055_read( dev )!=0 ) return K8055_ERROR;
    *data1 = (
                 ( ( dev->data_in[DIGITAL_INP_OFFSET] >> 4 ) & 0x03 ) |  /* Input 1 and 2 */
                 ( ( dev->data_in[DIGITAL_INP_OFFSET] << 2 ) & 0x04 ) |  /* Input 3 */
                 ( ( dev->data_in[DIGITAL_INP_OFFSET] >> 3 ) & 0x18 ) ); /* Input 4 and 5 */
    *data2 = dev->data_in[ANALOG_1_OFFSET];
    *data3 = dev->data_in[ANALOG_2_OFFSET];
    *data4 = *( ( short int* )( &dev->data_in[COUNTER_1_OFFSET] ) );
    *data5 = *( ( short int* )( &dev->data_in[COUNTER_2_OFFSET] ) );
    return 0;
}

int k8055_set_all_values( struct k8055_dev* dev, int digital_data, int ad_data1, int ad_data2 ) {
    dev->data_out[CMD_OFFSET] = CMD_SET_ANALOG_DIGITAL;
    dev->data_out[DIGITAL_OUT_OFFSET] = ( unsigned char )digital_data;
    dev->data_out[ANALOG_1_OFFSET] = ( unsigned char )ad_data1;
    dev->data_out[ANALOG_2_OFFSET] = ( unsigned char )ad_data2;
    return k8055_write( dev );
}

int k8055_reset_counter( struct k8055_dev* dev, int counter ) {
    if ( !( counter==1 || counter==2 ) ) return K8055_ERROR;
    dev->data_out[0] = 0x02 + ( unsigned char )counter;
    dev->data_out[3+counter] = 0x00;
    return k8055_write( dev );
}

int k8055_read_counter( struct k8055_dev* dev, int counter ) {
    if ( !( counter==1 || counter==2 ) ) return K8055_ERROR;
    if ( k8055_read( dev )!=0 ) return K8055_ERROR;
    if ( counter==1 ) {
        return *( ( short int* )( &dev->data_in[COUNTER_1_OFFSET] ) );
    } else {
        return *( ( short int* )( &dev->data_in[COUNTER_2_OFFSET] ) );
    }
}

int k8055_set_counter_debounce_time( struct k8055_dev* dev, int counter, int debounce_time ) {
    float value;
    if ( !( counter==1 || counter==2 ) ) return K8055_ERROR;
    dev->data_out[0] = ( unsigned char )counter;
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
    if ( debounce_time > 7450 ) debounce_time = 7450;
    value = sqrtf( debounce_time / 0.115 );
    /* simple round() function) */
    if ( value > ( ( int )value + 0.49999999 ) ) value+=1;
    dev->data_out[5+counter] = ( unsigned char )value;
    if ( debug ) fprintf( stderr, "Debouncetime%d value for k8055:%d\n",( int )counter, dev->data_out[5+counter] );
    return k8055_write( dev );
}

/** Velleman API ***************************************************************************************************************************/

#define K8055_MAX_DEV 4
static struct k8055_dev k8055d[K8055_MAX_DEV];
static struct k8055_dev* curr_dev;

char* Version( void ) {
    return( VERSION );
}
/* New function in version 2 of Velleman DLL, should return devices-found bitmask or 0*/
long SearchDevices( void ) {
    return k8055_search_devices();
}
/* Open device - scan through usb busses looking for the right device, claim it and then open the device */
int OpenDevice( long board_address ) {
    if( board_address<0 || board_address>=K8055_MAX_DEV ) return K8055_ERROR;
    if( k8055d[board_address].dev_no!=0 ) return board_address;
    int ret = k8055_open_device( &k8055d[board_address], board_address );
    if ( ret != K8055_ERROR ) {
        curr_dev = &k8055d[board_address];
        return ret;
    }
    return K8055_ERROR;
}
/* Close the Current device */
int CloseDevice() {
    return k8055_close_device( curr_dev );
}
/* New function in version 2 of Velleman DLL, should return deviceno if OK */
long SetCurrentDevice( long deviceno ) {
    if ( deviceno < 0 || deviceno >= K8055_MAX_DEV ) return K8055_ERROR;
    if ( k8055d[deviceno].dev_no == 0 ) return K8055_ERROR;
    curr_dev  = &k8055d[deviceno];
    return deviceno;
}
long ReadAnalogChannel( long channel ) {
    return k8055_read_analog_channel( curr_dev, channel );
}
int ReadAllAnalog( long* data1, long* data2 ) {
    int d1, d2;
    int r = k8055_read_all_analog( curr_dev, &d1, &d2 );
    *data1 = d1;
    *data2 = d2;
    return r;
}
int OutputAnalogChannel( long channel, long data ) {
    return k8055_write_analog_channel( curr_dev, channel, data );
}
int OutputAllAnalog( long data1, long data2 ) {
    return k8055_write_all_analog( curr_dev, data1, data2 );
}
int ClearAllAnalog() {
    return k8055_clear_all_analog( curr_dev );
}
int ClearAnalogChannel( long channel ) {
    return k8055_clear_analog_channel( curr_dev, channel );
}
int SetAnalogChannel( long channel ) {
    return k8055_set_analog_channel( curr_dev, channel );
}
int SetAllAnalog() {
    return k8055_set_all_analog( curr_dev );
}
int WriteAllDigital( long data ) {
    return k8055_write_all_digital( curr_dev, data );
}
int ClearDigitalChannel( long channel ) {
    return k8055_clear_digital_channel( curr_dev, channel );
}
int ClearAllDigital() {
    return k8055_clear_all_digital( curr_dev );
}
int SetDigitalChannel( long channel ) {
    return k8055_set_digital_channel( curr_dev, channel );
}
int SetAllDigital() {
    return k8055_set_all_digital( curr_dev );
}
int ReadDigitalChannel( long channel ) {
    return k8055_read_digital_channel( curr_dev, channel );
}
long ReadAllDigital() {
    return k8055_read_all_digital( curr_dev );
}
int ReadAllValues( long int* data1, long int* data2, long int* data3, long int* data4, long int* data5 ) {
    int d1, d2, d3, d4, d5;
    int r = k8055_read_all_values( curr_dev, &d1, &d2, &d3, &d4, &d5 );
    *data1 = d1;
    *data2 = d2;
    *data3 = d3;
    *data4 = d4;
    *data5 = d5;
    return r;
}
int SetAllValues( int DigitalData, int AdData1, int AdData2 ) {
    return k8055_set_all_values( curr_dev, DigitalData, AdData1, AdData2 );
}
int ResetCounter( long counter ) {
    return k8055_reset_counter( curr_dev, counter );
}
long ReadCounter( long counter ) {
    return k8055_read_counter( curr_dev, counter );
}
int SetCounterDebounceTime( long counter, long debounce_time ) {
    return k8055_set_counter_debounce_time( curr_dev, counter, debounce_time );
}
