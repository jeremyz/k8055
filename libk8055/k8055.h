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
#ifndef K8055_H
#define K8055_H 1

#ifdef __cplusplus
extern "C" {
#endif

#define PACKET_LEN 8

#define DIGITAL_INP_OFFSET 0
#define DIGITAL_OUT_OFFSET 1
#define ANALOG_1_OFFSET 2
#define ANALOG_2_OFFSET 3
#define COUNTER_1_OFFSET 4
#define COUNTER_2_OFFSET 6

    struct k8055_dev {
        int dev_no;
        int debug_level;
        struct libusb_context *usb_ctx;
        struct libusb_device_handle *usb_handle;
        unsigned char data_in[PACKET_LEN+1];
        unsigned char data_out[PACKET_LEN+1];
    };
    /* create/destroy struct */
    struct k8055_dev* k8055_alloc( void );
    void k8055_free( struct k8055_dev* dev );
    /* read data_in / write data_out */
    int k8055_read( struct k8055_dev* dev );
    int k8055_write( struct k8055_dev* dev );

    /* access output data */
    int k8055_digital_outputs( struct k8055_dev* dev );
    int k8055_digital_output_1( struct k8055_dev* dev );
    int k8055_digital_output_2( struct k8055_dev* dev );
    int k8055_digital_output_3( struct k8055_dev* dev );
    int k8055_digital_output_4( struct k8055_dev* dev );
    int k8055_digital_output_5( struct k8055_dev* dev );
    int k8055_analog_output_1( struct k8055_dev* dev );
    int k8055_analog_output_2( struct k8055_dev* dev );
    /* access input data */
    int k8055_digital_input_1( struct k8055_dev* dev );
    int k8055_digital_input_2( struct k8055_dev* dev );
    int k8055_digital_input_3( struct k8055_dev* dev );
    int k8055_digital_input_4( struct k8055_dev* dev );
    int k8055_digital_input_5( struct k8055_dev* dev );
    int k8055_analog_input_1( struct k8055_dev* dev );
    int k8055_analog_input_2( struct k8055_dev* dev );
    int k8055_counter_1( struct k8055_dev* dev );
    int k8055_counter_2( struct k8055_dev* dev );

    char* k8055_version( void );
    int k8055_search_devices( int verbose );

    int k8055_open_device( struct k8055_dev* dev, int board_address );
    int k8055_close_device( struct k8055_dev* dev );
    int k8055_set_debug( struct k8055_dev* dev, int level );
    int k8055_get_debug( struct k8055_dev* dev );

    /* analog outputs */
    int k8055_set_analog_channel( struct k8055_dev* dev, int channel );
    int k8055_clear_analog_channel( struct k8055_dev* dev, int channel );
    int k8055_write_analog_channel( struct k8055_dev* dev ,int channel, int data );
    int k8055_set_all_analog( struct k8055_dev* dev );
    int k8055_clear_all_analog( struct k8055_dev* dev );
    int k8055_write_all_analog( struct k8055_dev* dev, int data1, int data2 );
    /* analog inputs */
    int k8055_read_analog_channel( struct k8055_dev* dev, int channel );
    int k8055_read_all_analog( struct k8055_dev* dev, int* data1, int* data2 );
    /* digital outputs */
    int k8055_set_digital_channel( struct k8055_dev* dev, int channel );
    int k8055_clear_digital_channel( struct k8055_dev* dev, int channel );
    int k8055_set_all_digital( struct k8055_dev* dev );
    int k8055_clear_all_digital( struct k8055_dev* dev );
    int k8055_write_all_digital( struct k8055_dev* dev, int data );
    /* digital inputs */
    int k8055_read_digital_channel( struct k8055_dev* dev, int channel );
    int k8055_read_all_digital( struct k8055_dev* dev );

    int k8055_write_all_outputs( struct k8055_dev* dev, int digital, int analog1, int analog2 );
    int k8055_read_all_inputs( struct k8055_dev* dev, int* digital, int* analog1, int* analog2, int* counter1, int* counter2 );

    int k8055_reset_counter( struct k8055_dev* dev, int counter );
    int k8055_read_counter( struct k8055_dev* dev, int counter );

    int k8055_set_counter_debounce_time( struct k8055_dev* dev, int counter, int debounce_time );

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

#endif // K8055_H
