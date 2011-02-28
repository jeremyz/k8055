/* $Id: k8055.h,v 1.4 2008/05/21 20:25:51 mr_brain Exp $

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
*/
#ifdef __cplusplus
extern "C" {
#endif

#define PACKET_LEN 8

    struct k8055_dev {
        int dev_no;
        struct usb_dev_handle* device_handle;
        unsigned char data_in[PACKET_LEN+1];
        unsigned char data_out[PACKET_LEN+1];
    };

    long search_devices( void );
    int open_device( struct k8055_dev* dev, long board_address );
    int close_device( struct k8055_dev* dev );
    long read_analog_channel( struct k8055_dev* dev, long channel );
    int read_all_analog( struct k8055_dev* dev, long* data1, long* data2 );
    int output_analog_channel( struct k8055_dev* dev ,long channel, long data );
    int output_all_analog( struct k8055_dev* dev, long data1, long data2 );
    int clear_all_analog( struct k8055_dev* dev );
    int clear_analog_channel( struct k8055_dev* dev, long channel );
    int set_analog_channel( struct k8055_dev* dev, long channel );
    int set_all_analog( struct k8055_dev* dev );
    int write_all_digital( struct k8055_dev* dev, long data );
    int clear_digital_channel( struct k8055_dev* dev, long channel );
    int clear_all_digital( struct k8055_dev* dev );
    int set_digital_channel( struct k8055_dev* dev, long channel );
    int set_all_digital( struct k8055_dev* dev );
    int read_digital_channel( struct k8055_dev* dev, long channel );
    long read_all_digital( struct k8055_dev* dev );
    int read_all_values( struct k8055_dev* dev, long int* data1, long int* data2, long int* data3, long int* data4, long int* data5 );
    int set_all_values( struct k8055_dev* dev, int digital_data, int ad_data1, int ad_data2 );
    int reset_counter( struct k8055_dev* dev, long counter );
    long read_counter( struct k8055_dev* dev, long counter );
    int set_counter_debounce_time( struct k8055_dev* dev, long counter, long debounce_time );

    /* Velleman API */
    char* Version( void );
    long SearchDevices( void );
    int OpenDevice( long board_address );
    int CloseDevice();
    long SetCurrentDevice( long deviceno );
    long ReadAnalogChannel( long Channelno );
    int ReadAllAnalog( long* data1, long* data2 );
    int OutputAnalogChannel( long channel, long data );
    int OutputAllAnalog( long data1,long data2 );
    int ClearAllAnalog();
    int ClearAnalogChannel( long channel );
    int SetAnalogChannel( long channel );
    int SetAllAnalog();
    int WriteAllDigital( long data );
    int ClearDigitalChannel( long channel );
    int ClearAllDigital();
    int SetDigitalChannel( long channel );
    int SetAllDigital();
    int ReadDigitalChannel( long channel );
    long ReadAllDigital();
    int ReadAllValues ( long int*  data1, long int* data2, long int* data3, long int* data4, long int* data5 );
    int SetAllValues( int digitaldata, int addata1, int addata2 );
    int ResetCounter( long counternr );
    long ReadCounter( long counterno );
    int SetCounterDebounceTime( long counterno, long debouncetime );
#ifdef __cplusplus
}
#endif

