/*
 * beep.c
 *
 * Copyright (C) 2008 Sebastian Gottschall <gottschall@dd-wrt.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id:
 */
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <utils.h>
#include <wlutils.h>

void beep( int gpio, int time, int polarity )
{
    set_gpio( gpio, 1 - polarity );
    usleep( time * 1000 );
    set_gpio( gpio, 0 + polarity );
    usleep( time * 1000 );
}

int getmask_main( int argc, char **argv )
{

    unsigned int gpio;

    if( argc != 2 )
    {
	fprintf( stderr, "%s <netmask>\n", argv[0] );
	exit( 1 );
    }
    gpio = atoi( argv[2] );
    fprintf(stdout,"%d",getmask(gpio));

    return 0;
}
