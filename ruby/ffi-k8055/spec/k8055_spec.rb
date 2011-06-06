#! /usr/bin/env ruby
# -*- coding: UTF-8 -*-
#
require 'k8055'
#
describe K8055 do
    #
    it "version should work" do
        K8055::K8055.version.should == K8055.version
    end
    #
    it "search_devices should find one" do
        K8055.search_devices.should > 0
    end
    #
    describe K8055 do
        #
        before(:all) { 
            @k = K8055::K8055.new
            @k.open_device 0
        }
        after(:all) {
            @k.close_device
            @k.free
        }
        #
        it "analog output 1" do
            @k.k8055_set_analog_channel 1
            @k.k8055_analog_output_1.should == 255
            @k.k8055_write_analog_channel 1, 10
            @k.k8055_analog_output_1.should == 10
            @k.k8055_write_analog_channel 1, 250
            @k.k8055_analog_output_1.should == 250
            @k.k8055_clear_analog_channel 1
            @k.k8055_analog_output_1.should == 0
            @k.set_analog_channel 1
            @k.analog_output_1.should == 255
            @k.write_analog_channel 1, 10
            @k.analog_output_1.should == 10
            @k.write_analog_channel 1, 250
            @k.analog_output_1.should == 250
            @k.clear_analog_channel 1
            @k.analog_output_1.should == 0
        end
        it "analog output 2" do
            @k.k8055_set_analog_channel 2
            @k.k8055_analog_output_2.should == 255
            @k.k8055_write_analog_channel 2, 10
            @k.k8055_analog_output_2.should == 10
            @k.k8055_write_analog_channel 2, 250
            @k.k8055_analog_output_2.should == 250
            @k.k8055_clear_analog_channel 2
            @k.k8055_analog_output_2.should == 0
            @k.set_analog_channel 2
            @k.analog_output_2.should == 255
            @k.write_analog_channel 2, 10
            @k.analog_output_2.should == 10
            @k.write_analog_channel 2, 250
            @k.analog_output_2.should == 250
            @k.clear_analog_channel 2
            @k.analog_output_2.should == 0
        end
        #
        it "digital output 1" do
            @k.set_all_digital
            @k.digital_output_1.should == 1
            @k.clear_digital_channel 1
            @k.digital_output_1.should == 0
            @k.set_digital_channel 1
            @k.digital_output_1.should == 1
            @k.clear_all_digital
            @k.digital_output_1.should == 0
        end
        #
        it "digital output 2" do
            @k.set_all_digital
            @k.digital_output_2.should == 1
            @k.clear_digital_channel 2
            @k.digital_output_2.should == 0
            @k.set_digital_channel 2
            @k.digital_output_2.should == 1
            @k.clear_all_digital
            @k.digital_output_2.should == 0
        end
        #
        it "digital output 3" do
            @k.set_all_digital
            @k.digital_output_3.should == 1
            @k.clear_digital_channel 3
            @k.digital_output_3.should == 0
            @k.set_digital_channel 3
            @k.digital_output_3.should == 1
            @k.clear_all_digital
            @k.digital_output_3.should == 0
        end
        #
        it "digital output 4" do
            @k.set_all_digital
            @k.digital_output_4.should == 1
            @k.clear_digital_channel 4
            @k.digital_output_4.should == 0
            @k.set_digital_channel 4
            @k.digital_output_4.should == 1
            @k.clear_all_digital
            @k.digital_output_4.should == 0
        end
        #
        it "digital output 5" do
            @k.set_all_digital
            @k.digital_output_5.should == 1
            @k.clear_digital_channel 5
            @k.digital_output_5.should == 0
            @k.set_digital_channel 5
            @k.digital_output_5.should == 1
            @k.clear_all_digital
            @k.digital_output_5.should == 0
        end
        #
        it "outputs test loop" do
            @k.set_all_digital
            @k.set_all_analog
            @k.digital_outputs.should == 255
            @k.digital_output_1.should == 1
            @k.digital_output_2.should == 1
            @k.digital_output_3.should == 1
            @k.digital_output_4.should == 1
            @k.digital_output_5.should == 1
            @k.analog_output_1.should == 255
            @k.analog_output_2.should == 255
            (0..255).each do |n|
                @k.write_all_digital n
                @k.write_all_analog n, n
                @k.digital_outputs.should == n
            end
            @k.clear_all_digital
            @k.clear_all_analog
            @k.digital_outputs.should == 0
            @k.digital_output_1.should == 0
            @k.digital_output_2.should == 0
            @k.digital_output_3.should == 0
            @k.digital_output_4.should == 0
            @k.digital_output_5.should == 0
            @k.analog_output_1.should == 0
            @k.analog_output_2.should == 0
        end
    end
end
