/* $Id: main.c,v 1.3 2007/03/15 14:37:58 pjetur Exp $
 ****************************************************************************
 *   Copyright (C) 2004 by Nicolas Sutre                                    *
 *   nicolas.sutre@free.fr                                                  *
 *                                                                          *
 *   Copyright (C) 2005 by Bob Dempsey                                      *
 *   bdempsey_64@msn.com 						                            *
 *									                                        *
 *   Copyright (C) 2005 by Julien Etelain and Edward Nys		            *
 *   Converted to C							                                *
 *   Commented and improved by Julien Etelain <julien.etelain@utbm.fr>      *
 *                             Edward Nys <edward.ny@utbm.fr>               *
 *                                                                          *
 *   Copyleft (L) 2005 by Sven Lindberg					                    *
 *   k8055@k8055.mine.nu						                            *
 *   Give it up already :) Simplified (read improved..) and included 	    *
 *   with k8055 lib and with a functional man page			                *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <usb.h>
#include <assert.h>
#include <sys/time.h>
#include "k8055.h"

#define STR_BUFF 256
#define false 0
#define true 1

int debug = 0;
int ia1 = -1;
int ia2 = -1;
int id8 = -1;
int ipid = 0;
int numread = 1;
int dbt1 = -1;
int dbt2 = -1;
int resetcnt1 = false;
int resetcnt2 = false;
int delay = 0;

inline int str2int( char* text, int* i ) {
    return sscanf( text, "%d", i );
}

static void display_help ( char* params[] ) {
    printf( "K8055 version %s MrBrain Build\n",version() );
    printf( "Copyright (C) 2004 by Nicolas Sutre\n" );
    printf( "Copyright (C) 2005 by Bob Dempsey\n" );
    printf( "Copyright (C) 2005 by Julien Etelain and Edward Nys\n" );
    printf( "Copyleft (L) 2005 by Sven Lindberg\n" );
    printf( "\n" );
    printf( "Syntax : %s [-p:(number)] [-d:(value)] [-a1:(value)] [-a2:(value)]\n",params[0] );
    printf( "             [-num:(number) [-delay:(number)] [-dbt1:(value)]\n" );
    printf( "             [-dbt2:(value)] [-reset1] [-reset2] [-debug]\n" );
    printf( "	-p:(number)	Set board number\n" );
    printf( "	-d:(value)	Set digital output value (8 bits in decimal)\n" );
    printf( "	-a1:(value)	Set analog output 1 value (0-255)\n" );
    printf( "	-a2:(value)	Set analog output 2 value (0-255)\n" );
    printf( "	-num:(number)   Set number of measures\n" );
    printf( "	-delay:(number) Set delay between two measure (in msec)\n" );
    printf( "	-dbt1:(value)   Set debounce time for counter 1 (in msec)\n" );
    printf( "	-dbt2:(value)   Set debounce time for counter 2 (in msec)\n" );
    printf( "	-reset1		Reset counter 1\n" );
    printf( "	-reset2		Reset counter 2\n" );
    printf( "	-debug		Activate debug mode\n" );
    printf( "Example : %s -p:1 -d:147 -a1:25 -a2:203\n",params[0] );
    printf( "\n" );
    printf( "Output : (timestamp);(digital);(analog 1);(analog 2);(counter 1);(counter 2)\n" );
    printf( "Note : timestamp is the number of msec when data is read since program start\n" );
    printf( "Example : 499;16;128;230;9;8\n" );
    printf( "499 : Measure done 499 msec after program start\n" );
}

int read_param( int argc, char* params[] ) {
    int erreurParam = false;
    int i;
    ipid = 0;
    for ( i=1; i<argc; i++ ) {
        if ( !strncmp( params[i],"-p:",3 ) &&
                !str2int( params[i]+3,&ipid ) ) erreurParam = true;
        else if ( !strncmp( params[i],"-a1:",4 )  &&
                  !str2int( params[i]+4,&ia1 ) ) erreurParam = true;
        else if ( !strncmp( params[i],"-a2:",4 ) &&
                  !str2int( params[i]+4,&ia2 ) ) erreurParam = true;
        else if ( !strncmp( params[i],"-d:",3 ) &&
                  !str2int( params[i]+3,&id8 ) ) erreurParam = true;
        else if ( !strncmp( params[i],"-num:",5 ) &&
                  !str2int( params[i]+5,&numread ) ) erreurParam = true;
        else if ( !strncmp( params[i],"-delay:",7 ) &&
                  !str2int( params[i]+7,&delay ) ) erreurParam = true;
        else if ( !strncmp( params[i],"-dbt1:",6 ) &&
                  !str2int( params[i]+6,&dbt1 ) ) erreurParam = true;
        else if ( !strncmp( params[i],"-dbt2:",6 ) &&
                  !str2int( params[i]+6,&dbt2 ) ) erreurParam = true;
        else if ( !strcmp( params[i],"-debug" ) ) {
            debug = 1;
            set_debug_on();
        } else if ( !strcmp( params[i],"-reset1" ) ) resetcnt1 = true;
        else if ( !strcmp( params[i],"-reset2" ) ) resetcnt2 = true;
        else if ( !strcmp( params[i],"--help" ) ) {
            display_help( params );
            return false;
        }
    }
    if ( debug )
        fprintf( stderr,"Parameters : Card=%d Analog1=%d Analog2=%d Digital=%d\n",ipid,ia1,ia2,id8 );
    if ( ipid<0 || ipid>3 ) {
        printf( "Invalid board address!\n" );
        return -1;
    }
    if ( erreurParam ) {
        printf( "Invalid or incomplete options\n" );
        display_help( params );
        return false;
    }
    return true;
}

inline unsigned long int time_msec ( void ) {
    struct timeval t;
    struct timezone tz;
    gettimeofday ( &t,&tz );
    return ( 1000*t.tv_sec )+( t.tv_usec/1000 );
}

int main ( int argc,char* params[] ) {
    int i,result;
    long d=0;
    long a1=0,a2=0;
    long c1=0, c2=0;
    unsigned long int start,mstart=0,lastcall=0;
    start = time_msec();
    if ( read_param( argc,params ) ) {
        /*
         * Initialise USB system
         * and enable debug mode
         */
        if ( debug )
            usb_set_debug( 2 );
        struct k8055_dev dev;
        if ( open_device( &dev, ipid )<0 ) {
            printf( "Could not open the k8055 (port:%d)\nPlease ensure that the device is correctly connected.\n",ipid );
            return ( EXIT_FAILURE );
        } else {
            if ( resetcnt1 )
                reset_counter( &dev,1 );
            if ( resetcnt2 )
                reset_counter( &dev,2 );
            if ( dbt1 != -1 )
                set_counter_debounce_time( &dev,1,dbt1 );
            if ( dbt2 != -1 )
                set_counter_debounce_time( &dev,2,dbt1 );
            if ( ( ia1!=-1 ) && ( ia2!=-1 ) && ( id8!=-1 ) ) {
                result = set_all_values( &dev,id8,ia1,ia2 );
                if ( debug ) printf( "SetAllValues=%d - Digital:%d, analog1:%d, analog2:%d\n",result,id8,ia1,ia2 );
            } else if ( ( id8 != -1 ) && ( ia1!=-1 ) ) {
                result = set_all_values( &dev,id8,ia1,0 );
                if ( debug ) printf( "SetAllValues=%d - Digital:%d, analog1:%d\n",result,id8,ia1 );
            } else if ( ( id8 != -1 ) && ( ia2!=-1 ) ) {
                result = set_all_values( &dev,id8,0,ia2 );
                if ( debug ) printf( "SetAllValues=%d - Digital:%d, analog2:%d\n",result,id8,ia2 );
            } else if ( ( ia1 != -1 ) && ( ia2!=-1 ) ) {
                result = set_all_values( &dev,0,ia1,ia2 );
                if ( debug ) printf( "SetAllValues=%d - analog1:%d, analog2:%d\n",result,ia1,ia2 );
            } else {
                if ( ia1!=-1 ) {
                    result=output_analog_channel( &dev,1,ia1 );
                    if ( debug ) printf( "Set analog1:%d=>%d\n",ia1,result );
                }
                if ( ia2!=-1 ) {
                    result=output_analog_channel( &dev,2,ia2 );
                    if ( debug ) printf( "Set analog2:%d=>%d\n",ia2,result );
                }
                if ( id8!=-1 ) {
                    result=write_all_digital( &dev,( long )id8 );
                    if ( debug ) printf( "Set digital:%d=>%d\n",id8,result );
                }
            }
            mstart = time_msec();
            for ( i=0; i<numread; i++ ) {
                if ( delay ) {
                    while ( time_msec()-mstart < i*delay );
                }
                read_all_values( &dev,&d,&a1,&a2,&c1,&c2 );
                lastcall = time_msec();
                printf( "%d;%d;%d;%d;%d;%d\n", ( int )( lastcall-start ),( int )d, ( int )a1, ( int )a2,( int )c1,( int )c2 );
            }
            close_device( &dev );
        }
    }
    return EXIT_SUCCESS;
}
