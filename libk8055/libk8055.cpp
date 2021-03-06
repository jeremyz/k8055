/*
 * This file is part of the libk8055 Library.
 *
 * The libk8055 Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The libk8055 Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * http://opensource.org/licenses/
 *
 * Copyright (C) 2011 by Jérémy Zurcher
 *
 */

#include "k8055++.h"

K8055::K8055() {
}

K8055::~K8055() {
    k8055_close_device( &dev );
}

int K8055::read( void ) {
    return k8055_read( &dev );
}

int K8055::write( void ) {
    return k8055_write( &dev );
}

int K8055::digital_outputs( void ) {
    return k8055_digital_outputs( &dev );
}

int K8055::digital_output_1( void ) {
    return k8055_digital_output_1( &dev );
}

int K8055::digital_output_2( void ) {
    return k8055_digital_output_2( &dev );
}

int K8055::digital_output_3( void ) {
    return k8055_digital_output_3( &dev );
}

int K8055::digital_output_4( void ) {
    return k8055_digital_output_4( &dev );
}

int K8055::digital_output_5( void ) {
    return k8055_digital_output_5( &dev );
}

int K8055::analog_output_1( void ) {
    return k8055_analog_output_1( &dev );
}

int K8055::analog_output_2( void ) {
    return k8055_analog_input_2( &dev );
}

int K8055::digital_input_1( void ) {
    return k8055_digital_input_1( &dev );
}

int K8055::digital_input_2( void ) {
    return k8055_digital_input_2( &dev );
}

int K8055::digital_input_3( void ) {
    return k8055_digital_input_3( &dev );
}

int K8055::digital_input_4( void ) {
    return k8055_digital_input_4( &dev );
}

int K8055::digital_input_5( void ) {
    return k8055_digital_input_5( &dev );
}

int K8055::analog_input_1( void ) {
    return k8055_analog_input_1( &dev );
}

int K8055::analog_input_2( void ) {
    return k8055_analog_input_2( &dev );
}

int K8055::counter_1( void ) {
    return k8055_counter_1( &dev );
}

int K8055::counter_2( void ) {
    return k8055_counter_2( &dev );
}

char* K8055::Version( void ) {
    return k8055_version();
}

int K8055::SearchDevices( void ) {
    return k8055_search_devices(0);
}

int K8055::OpenDevice( int board_address ) {
    return k8055_open_device( &dev, board_address );
}

int K8055::CloseDevice() {
    return k8055_close_device( &dev );
}

int K8055::SetAnalogChannel( int channel ) {
    return k8055_set_analog_channel( &dev, channel );
}

int K8055::ClearAnalogChannel( int channel ) {
    return k8055_clear_analog_channel( &dev, channel );
}

int K8055::ReadAnalogChannel( int channel ) {
    return k8055_read_analog_channel( &dev, channel );
}

int K8055::WriteAnalogChannel( int channel, int data ) {
    return k8055_write_analog_channel( &dev, channel, data );
}

int K8055::SetAllAnalog() {
    return k8055_set_all_analog( &dev );
}
int K8055::ClearAllAnalog() {
    return k8055_clear_all_analog( &dev );
}

int K8055::ReadAllAnalog( int* data1, int* data2 ) {
    return k8055_read_all_analog( &dev, data1, data2 );
}

int K8055::WriteAllAnalog( int data1,int data2 ) {
    return k8055_write_all_analog( &dev, data1, data2 );
}

int K8055::SetDigitalChannel( int channel ) {
    return k8055_set_digital_channel( &dev, channel );
}

int K8055::ReadDigitalChannel( int channel ) {
    return k8055_read_digital_channel( &dev, channel );
}

int K8055::ClearDigitalChannel( int channel ) {
    return k8055_clear_digital_channel( &dev, channel );
}

int K8055::SetAllDigital() {
    return k8055_set_all_digital( &dev );
}

int K8055::ClearAllDigital() {
    return k8055_clear_all_digital( &dev );
}

int K8055::ReadAllDigital() {
    return k8055_read_all_digital( &dev );
}

int K8055::WriteAllDigital( int data ) {
    return k8055_write_all_digital( &dev, data );
}

int K8055::WriteAllOutputs( int digital, int analog1, int analog2 ) {
    return k8055_write_all_outputs( &dev, digital, analog1, analog2 );
}

int K8055::ReadAllInputs ( int*  digital, int* analog1, int* analog2, int* counter1, int* counter2 ) {
    return k8055_read_all_inputs( &dev, digital, analog1, analog2, counter1, counter2 );
}

int K8055::ResetCounter( int counter ) {
    return k8055_reset_counter( &dev, counter );
}

int K8055::ReadCounter( int counter ) {
    return k8055_read_counter( &dev, counter );
}

int K8055::SetCounterDebounceTime( int counter, int debounce_time ) {
    return k8055_set_counter_debounce_time( &dev, counter, debounce_time );
}

