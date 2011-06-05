/* $Id: libk8055.i,v 1.3 2007/03/28 10:08:13 pjetur Exp $

   Copyright (C) 2007  by Pjetur G. Hjaltason


   Python wrapper for the libk8055 library using "swig"
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
%module pyk8055
%include "typemaps.i"


%inline %{

%}

%{
extern struct k8055_dev* k8055_alloc( void );
extern void k8055_free( struct k8055_dev* dev );
/*
extern int k8055_read( struct k8055_dev* dev );
extern int k8055_write( struct k8055_dev* dev );
extern int k8055_digital_1( struct k8055_dev* dev );
extern int k8055_digital_2( struct k8055_dev* dev );
extern int k8055_digital_3( struct k8055_dev* dev );
extern int k8055_digital_4( struct k8055_dev* dev );
extern int k8055_digital_5( struct k8055_dev* dev );
extern int k8055_analog_1( struct k8055_dev* dev );
extern int k8055_analog_2( struct k8055_dev* dev );
extern int k8055_counter_1( struct k8055_dev* dev );
extern int k8055_counter_2( struct k8055_dev* dev );
*/
/*extern char* k8055_version( void );*/
extern void k8055_set_debug( int status );
extern void k8055_get_debug( void );
extern int k8055_search_devices( void );
extern int k8055_open_device( struct k8055_dev* dev, int board_address );
extern int k8055_close_device( struct k8055_dev* dev );
extern int k8055_set_analog_channel( struct k8055_dev* dev, int channel );
extern int k8055_clear_analog_channel( struct k8055_dev* dev, int channel );
extern int k8055_read_analog_channel( struct k8055_dev* dev, int channel );
extern int k8055_write_analog_channel( struct k8055_dev* dev ,int channel, int data );
extern int k8055_set_all_analog( struct k8055_dev* dev );
extern int k8055_clear_all_analog( struct k8055_dev* dev );
extern int k8055_read_all_analog( struct k8055_dev* dev, int* data1, int* data2 );
extern int k8055_write_all_analog( struct k8055_dev* dev, int data1, int data2 );
extern int k8055_set_digital_channel( struct k8055_dev* dev, int channel );
extern int k8055_clear_digital_channel( struct k8055_dev* dev, int channel );
extern int k8055_read_digital_channel( struct k8055_dev* dev, int channel );
extern int k8055_set_all_digital( struct k8055_dev* dev );
extern int k8055_clear_all_digital( struct k8055_dev* dev );
extern int k8055_read_all_digital( struct k8055_dev* dev );
/*extern int k8055_write_all_digital( struct k8055_dev* dev, int data );*/
extern int k8055_set_all_values( struct k8055_dev* dev, int d_data, int a_data1, int a_data2 );
extern int k8055_read_all_values( struct k8055_dev* dev, int* data1, int* data2, int* data3, int* data4, int* data5 );
extern int k8055_reset_counter( struct k8055_dev* dev, int counter );
extern int k8055_read_counter( struct k8055_dev* dev, int counter );
extern int k8055_set_counter_debounce_time( struct k8055_dev* dev, int counter, int debounce_time );
%}
/* 

And here we create the class interface to the library

*/
%pythoncode %{
K8055_ERROR = -1

class k8055:
    "Class interface to the libk8055 library"
    def __init__(self,BoardAddress=None,debug=False):
        """Constructor, optional board open

         k=k8055()  # Does not connect to board
         k=k8055(1) # Init class and connect to board 1
         k=k8055(1,True) # connect to board 0 and enable debugging

        """
        self.dev = _pyk8055.k8055_alloc()   # TODO how to free ??
        if debug == False:
            self.DebugOff()
        else:
            self.DebugOn()
        if BoardAddress != None:
            self.OpenDevice(BoardAddress)

    def __finalize__(self):
        """Destructor"""
        _pyk8055.k8055_free(self.dev)

    #def Version(self):
    #    return _pyk8055.k8055_version();

    def DebugOn(self):
        return _pyk8055.k8055_set_debug(1);

    def DebugOff(self):
        return _pyk8055.k8055_set_debug(0);

    def SearchDevices(self):
        return _pyk8055.k8055_search_devices()

    def OpenDevice(self,BoardAddress):
        """Open the connection to K8055

        k=k8055()
        try;
           k.OpenDevice(0) # possible (address 0/1/2/3)
        except IOError:
            ...
        returns 0 if OK,
        Throws IOError if invalid board or not accessible
        """
        if _pyk8055.open_device(self.dev,BoardAddress) == K8055_ERROR:
            raise IOError, "Could not open device"
        # print "Open OK " + str(self.dev)
        return 0

    def CloseDevice(self):
        """Close the connection to K8055

        k.CloseDevice()

        retuns 0 if OK else -1
        """
        return _pyk8055.k8055_close_device(self.dev)

    def SetAnalogChannel(self,Channel):
        """Set analog output channel value to 255"""
        return _pyk8055.k8055_set_analog_channel(self.dev,Channel)

    def ClearAnalogChannel(self,Channel):
        """Set analog output channel value to 0"""
        return _pyk8055.k8055_clear_analog_channel(self.dev,Channel)

    def ReadAnalogChannel(self,Channel):
        """Read data from analog input channel (1/2)"""
        return _pyk8055.k8055_read_analog_channel(self.dev,Channel)

    def WriteAnalogChannel(self,Channel,value=0):
        """Set analog output channel value, default 0 (0-255)"""
        return _pyk8055.k8055_write_analog_channel(self.dev,Channel,value)

    def SetAllAnalog(self):
        """Set both analog output channels at once to 255"""
        return _pyk8055.k8055.set_all_analog(self.dev)

    def ClearAllAnalog(self):
        """Set both analog output channels at once to 0"""
        return _pyk8055.k8055.clear_all_analog(self.dev)

    def ReadAllAnalog(self):
        """Read data from both analog input channels at once

        Returns list, [return-value,channel_data1, channel_data2]
        """
        return _pyk8055.k8055_read_all_analog(self.dev)

    def WriteAllAnalog(self,data1,data2):
        """Set both analog output channels at once (0-255,0-255)"""
        return _pyk8055.k8055_write_all_analog(self.dev,data1,data2)

    def SetDigitalChannel(self,Channel):
        """Set digital output channel (1-8) high (1)"""
        return _pyk8055.k8055_set_digital_channel(self.dev,Channel)

    def ClearDigitalChannel(self,Channel):
        """Set digital output channel (1-8) low (0)"""
        return _pyk8055.k8055_clear_digital_channel(self.dev,Channel)

    def ReadDigitalChannel(self,Channel):
        """Read digital input channel (1-5), returns 0/1 (-1 on error)"""
        return _pyk8055.k8055_read_digital_channel(self.dev,Channel)

    def SetAllDigital(self):
        """Set all digital output channels high (1)"""
        return _pyk8055.k8055_set_all_digital(self.dev)

    def ClearAllDigital(self):
        """Set all digital output channels low (0)"""
        return _pyk8055.k8055_clear_all_digital(self.dev)

    def ReadAllDigital(self):
        """Read all digital input channels - bitmask

        returns mask 0x00 - 0x1f, 0x01 as channel 1, 0x02 as channel 2...
        retuns -1 on error

        """
        return _pyk8055.k8055_read_all_digital(self.dev)

    #def WriteAllDigital(self,data):
    #    """Write digital output channel bitmask (0-255)"""
    #    return _pyk8055.k8055_write_all_digital(self.dev,data)

    def SetAllValues(self,ddata, adata1, adata2):
        """Write digital output channel bitmask (0-255) and both analog channels data (0-255,0-255)"""
        return _pyk8055.k8055_set_all_values(self.dev,ddata, adata1, adata2)

    def ReadAllValues(self):
        """Read data from all input channels at once

        Returns list, [return-value,digital input data, analog channel_data1, analog channel_data2, counter1, counter2]

        """
        return _pyk8055.k8055_read_all_values(self.dev)

    def ResetCounter(self,CounterNo):
        """Reset input counter (1/2), input channel 1/2"""
        return _pyk8055.k8055_reset_Counter(self.dev,CounterNo)

    def ReadCounter(self,CounterNo):
        """Read input counter (1/2), input channel 1/2"""
        return _pyk8055.k8055_read_counter(self.dev,CounterNo)

    def SetCounterDebounceTime(self,CounterNo, DebounceTime):
        """Set counter debounce time on counter 1/2 (1-7450 ms)"""
        return _pyk8055.k8055_set_counter_debounce_time(self.dev,CounterNo,DebounceTime)
%}
extern struct k8055_dev* k8055_alloc( void );
extern void k8055_free( struct k8055_dev* dev );
/*
extern int k8055_read( struct k8055_dev* dev );
extern int k8055_write( struct k8055_dev* dev );
extern int k8055_digital_1( struct k8055_dev* dev );
extern int k8055_digital_2( struct k8055_dev* dev );
extern int k8055_digital_3( struct k8055_dev* dev );
extern int k8055_digital_4( struct k8055_dev* dev );
extern int k8055_digital_5( struct k8055_dev* dev );
extern int k8055_analog_1( struct k8055_dev* dev );
extern int k8055_analog_2( struct k8055_dev* dev );
extern int k8055_counter_1( struct k8055_dev* dev );
extern int k8055_counter_2( struct k8055_dev* dev );
*/
/*extern char* k8055_version( void );*/
extern void k8055_set_debug( int status );
extern void k8055_get_debug( void );
extern int k8055_search_devices( void );
extern int k8055_open_device( struct k8055_dev* dev, int board_address );
extern int k8055_close_device( struct k8055_dev* dev );
extern int k8055_set_analog_channel( struct k8055_dev* dev, int channel );
extern int k8055_clear_analog_channel( struct k8055_dev* dev, int channel );
extern int k8055_read_analog_channel( struct k8055_dev* dev, int channel );
extern int k8055_write_analog_channel( struct k8055_dev* dev ,int channel, int data );
extern int k8055_set_all_analog( struct k8055_dev* dev );
extern int k8055_clear_all_analog( struct k8055_dev* dev );
extern int k8055_read_all_analog( struct k8055_dev* dev, int* data1, int* data2 );
extern int k8055_write_all_analog( struct k8055_dev* dev, int data1, int data2 );
extern int k8055_set_digital_channel( struct k8055_dev* dev, int channel );
extern int k8055_clear_digital_channel( struct k8055_dev* dev, int channel );
extern int k8055_read_digital_channel( struct k8055_dev* dev, int channel );
extern int k8055_set_all_digital( struct k8055_dev* dev );
extern int k8055_clear_all_digital( struct k8055_dev* dev );
extern int k8055_read_all_digital( struct k8055_dev* dev );
/*extern int k8055_write_all_digital( struct k8055_dev* dev, int data );*/
extern int k8055_set_all_values( struct k8055_dev* dev, int d_data, int a_data1, int a_data2 );
extern int k8055_read_all_values( struct k8055_dev* dev, int* data1, int* data2, int* data3, int* data4, int* data5 );
extern int k8055_reset_counter( struct k8055_dev* dev, int counter );
extern int k8055_read_counter( struct k8055_dev* dev, int counter );
extern int k8055_set_counter_debounce_time( struct k8055_dev* dev, int counter, int debounce_time );
