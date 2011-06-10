#! /usr/bin/env ruby
# -*- coding: UTF-8 -*-
#
require 'ffi'
#
module K8055
    #
    module Native
        class K8055Struct < FFI::Struct
            layout  :dev_no,        :int,
                    :usb_ctx,       :pointer,
                    :usb_handle,    :pointer,
                    :data_in,       :pointer,
                    :data_out,      :pointer
        end
        #
        extend FFI::Library
        #
        ffi_lib 'k8055'
        #
        typedef :pointer, :k8055_dev
        #
        [
            [ :k8055_alloc, [], :k8055_dev],
            [ :k8055_free, [:k8055_dev], :void],
            [ :k8055_read, [:k8055_dev], :int],
            [ :k8055_write, [:k8055_dev], :int],
            [ :k8055_digital_outputs, [:k8055_dev], :int],
            [ :k8055_digital_output_1, [:k8055_dev], :int],
            [ :k8055_digital_output_2, [:k8055_dev], :int],
            [ :k8055_digital_output_3, [:k8055_dev], :int],
            [ :k8055_digital_output_4, [:k8055_dev], :int],
            [ :k8055_digital_output_5, [:k8055_dev], :int],
            [ :k8055_analog_output_1, [:k8055_dev], :int],
            [ :k8055_analog_output_2, [:k8055_dev], :int],
            [ :k8055_digital_input_1, [:k8055_dev], :int],
            [ :k8055_digital_input_2, [:k8055_dev], :int],
            [ :k8055_digital_input_3, [:k8055_dev], :int],
            [ :k8055_digital_input_4, [:k8055_dev], :int],
            [ :k8055_digital_input_5, [:k8055_dev], :int],
            [ :k8055_analog_input_1, [:k8055_dev], :int],
            [ :k8055_analog_input_2, [:k8055_dev], :int],
            [ :k8055_counter_1, [:k8055_dev], :int],
            [ :k8055_counter_2, [:k8055_dev], :int],
            [ :k8055_version, [], :string],
            [ :k8055_set_debug, [:int], :int],
            [ :k8055_get_debug, [], :int],
            [ :k8055_search_devices, [], :int],
            [ :k8055_open_device, [:k8055_dev,:int], :int],
            [ :k8055_close_device, [:k8055_dev], :int],
            [ :k8055_set_analog_channel, [:k8055_dev,:int], :int],
            [ :k8055_clear_analog_channel, [:k8055_dev,:int], :int],
            [ :k8055_read_analog_channel, [:k8055_dev,:int], :int],
            [ :k8055_write_analog_channel, [:k8055_dev,:int,:int], :int],
            [ :k8055_set_all_analog, [:k8055_dev], :int],
            [ :k8055_clear_all_analog, [:k8055_dev], :int],
            [ :k8055_read_all_analog, [:k8055_dev,:pointer,:pointer], :int],
            [ :k8055_write_all_analog, [:k8055_dev,:int,:int], :int],
            [ :k8055_set_digital_channel, [:k8055_dev,:int], :int],
            [ :k8055_clear_digital_channel, [:k8055_dev,:int], :int],
            [ :k8055_read_digital_channel, [:k8055_dev,:int], :int],
            [ :k8055_set_all_digital, [:k8055_dev], :int],
            [ :k8055_clear_all_digital, [:k8055_dev], :int],
            [ :k8055_read_all_digital, [:k8055_dev], :int],
            [ :k8055_write_all_digital, [:k8055_dev,:int], :int],
            [ :k8055_write_all_outputs, [:k8055_dev,:int,:int,:int], :int],
            [ :k8055_read_all_inputs, [:k8055_dev,:pointer,:pointer,:pointer,:pointer,:pointer], :int],
            [ :k8055_reset_counter, [:k8055_dev,:int], :int],
            [ :k8055_read_counter, [:k8055_dev,:int], :int]
        ].each do |func|
            begin
                attach_function(*func)
            rescue Object => e
                puts "Could not attach #{func} #{e.message}"
            end
        end
        #
    end
    #
    def self.version
        Native.k8055_version
    end
    #
    def self.search_devices
        Native.k8055_search_devices
    end
    #
    class K8055
        def initialize n=nil
            @dev = FFI::AutoPointer.new Native.k8055_alloc, K8055.method(:release)
            open n unless n.nil?
        end
        def self.release p
            Native.k8055_free p unless p.nil?
        end
        def free
            Native.k8055_close_device @dev
            @dev.autorelease=false if not @dev.nil?
            K8055.release @dev
            @dev=nil
        end
        def self.version
            Native.k8055_version
        end
        def k8055_read_all_analog
            a1 = FFI::MemoryPointer.new :int
            a2 = FFI::MemoryPointer.new :int
            Native.k8055_read_all_analog @dev, a1, a2
            [ a1.read_int, a2.read_int ]
        end
        alias :read_all_analog :k8055_read_all_analog
        def k8055_read_all_values
            a1 = FFI::MemoryPointer.new :int
            a2 = FFI::MemoryPointer.new :int
            Native.k8055_read_all_analog @dev, a1, a2
            [ a1.read_int, a2.read_int ]
        end
        alias :read_all_values :k8055_read_all_values
        def method_missing m, *args, &block
            m_s = m.to_s
            meth = (
                if Native.respond_to? m_s
                    m_s
                elsif Native.respond_to? 'k8055_'+m_s
                    'k8055_'+m_s
                else
                    nil
                end
            )
            if not meth.nil?
#                self.class.class_eval "def #{m} *args, &block; r=Native.#{meth}(@dev,*args); yield r if block_given?; r; end"
                self.class.class_eval "def #{m} *args, &block; Native.#{meth}(@dev,*args); end"
                return self.send m, *args, &block
            end
            Kernel.raise NameError.new "#{self.class.name} is unable to resolve #{m}"
        end
    end
    #
end
#
# EOF
