The k8055 require libusb-0.1.9 or newer and kernel 2.4.18 or never.

install:
	# make all
	# make install
	update ld.so.conf with "/usr/local/lib" if not there or run
	  env-update at gentoo systems	

uninstall:
	# make uninstall

Check out the Makefile for all different make rules...

User access to the k8055 device:
	read comments in velleman.rules

Using the k8055 library:
	in the program file "#include <k8055.h>" and compile with -lk8055 and
	-lusb, e.g gcc -lusb -lk8055 main.c -o k8055_rocks

Python part
	Python makes use of SWIG interface language, for setup and install
	see http://www.swig.org

	There might be a prepared package for your distribution, use
	emerge swig, apt-get install swig, urpmi swig, rpm -Uvh swig_package
	Or whatever your favorite distribution provides.
	Then, from the library source directory: 

	# make pylib

	You can then run the python programs from within the pyk8055 directory

	For global install, run as root:
	# make pyinstall
	This should install your package in the proper site-package dircetory
	globally accessible for the python interpreter
