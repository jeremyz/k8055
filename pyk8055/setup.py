#!/usr/bin/env python

import os,sys,string
from distutils.core import setup, Extension

if 'VERSION' in os.environ:
    version=os.environ['VERSION']
else:
    from subprocess import *
    try:
        major = Popen(["grep '(VERSION_MAJOR' ../CMakeLists.txt  | cut -d '\"' -f 2 | tr -d '\n'"], stdout=PIPE, shell=True).communicate()[0]
        minor = Popen(["grep '(VERSION_MINOR' ../CMakeLists.txt  | cut -d '\"' -f 2 | tr -d '\n'"], stdout=PIPE, shell=True).communicate()[0]
        patch = Popen(["grep '(VERSION_PATCH' ../CMakeLists.txt  | cut -d '\"' -f 2 | tr -d '\n'"], stdout=PIPE, shell=True).communicate()[0]
        version = "%s.%s.%s" % (major,minor,patch)
    except:
        version='?.?'

build_modules = [Extension('_pyk8055',
    define_macros = [('VERSION', "\"%s\"" % str(version))],
    libraries=["usb"],
    extra_compile_args=['-std=c99'],
    include_dirs=["/usr/include/libusb-1.0","/usr/local/include/libusb-1.0"],
    sources=['libk8055.i',"../libk8055/libk8055.c"])]

setup(
    name='pyk8055',
    version=version,
    author='Pjetur G. Hjaltason',
    author_email='pjetur@pjetur.net',
    description='K8055 library wrapper',
    url='http://libk8055.sourceforge.net/',
    ext_modules =build_modules,
    py_modules=['pyk8055']
)
