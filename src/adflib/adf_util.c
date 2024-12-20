/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 *
 *  adf_util.c
 *
 *  $Id$
 *
 *  This file is part of ADFLib.
 *
 *  ADFLib is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  ADFLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Foobar; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdio.h>
#include <time.h>

#include "adf_util.h"
#include "adf_err.h"
#include "adf_vol.h"
// #include "adf_env.h"



/*
 * swLong
 *
 * write an uint32_t value (val) (in) 
 * to an uint8_t* buffer (buf) (out)
 * 
 * used in adfWrite----Block() functions
 */
void swLong ( uint8_t * const buf,
              const uint32_t  val )
{
	buf[0]= (uint8_t)((val & 0xff000000) >>24UL);
	buf[1]= (uint8_t)((val & 0x00ff0000) >>16UL);
	buf[2]= (uint8_t)((val & 0x0000ff00) >>8UL);
	buf[3]= (uint8_t)(val & 0x000000ff);
}

void swShort ( uint8_t * const buf,
               const uint16_t  val )
{
    buf[0] = (uint8_t) ( (val & 0xff00u) >> 8UL );
    buf[1] = ( val & 0x00ffu );
}

/*
 * Days2Date
 *
 * amiga disk date format (days) to normal dd/mm/yy format (out)
 */

void adfDays2Date ( int32_t       days,
                    int * const   yy,
                    int * const   mm,
                    int * const   dd )
{
    int y,m;
    int nd;
    int jm[12]={ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    /* 0 = 1 Jan 1978,  6988 = 18 feb 1997 */

    /*--- year ---*/
    y=1978;
    if (adfIsLeap(y))
        nd=366;
    else
        nd=365;
    while( days >= nd ) {
        days-=nd;
        y++;
        if (adfIsLeap(y))
            nd=366;
        else
            nd=365;
    }


    /*--- month ---*/
    m=1;
    if (adfIsLeap(y))
        jm[2-1]=29;
    while( days >= jm[m-1] ) {
        days-=jm[m-1];
        m++;
    }

    *yy=y;
    *mm=m;
    *dd=days+1;
}


/*
 * IsLeap
 *
 * true if a year (y) is leap
 */
BOOL adfIsLeap ( const int y )
{
    return( (BOOL) ( !(y%100) ? !(y%400) : !(y%4) ) );
}


/*
 * adfCurrentDateTime
 *
 * return the current system date and time
 */
struct DateTime adfGiveCurrentTime ( void )
{
    struct tm *local;
    time_t cal;
    struct DateTime r;

    time(&cal);
    local=localtime(&cal);

    r.year=local->tm_year;         /* since 1900 */
    r.mon=local->tm_mon+1;
    r.day=local->tm_mday;
    r.hour=local->tm_hour;
    r.min=local->tm_min;
    r.sec=local->tm_sec;

    return(r);
}


/*
 * adfTime2AmigaTime
 *
 * converts date and time (dt) into Amiga format : day, min, ticks
 */
void adfTime2AmigaTime ( struct DateTime dt,
                         int32_t * const day,
                         int32_t * const min,
                         int32_t * const ticks )
{
    int jm[12]={ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


    *min= dt.hour*60 + dt.min;                /* mins */
    *ticks= dt.sec*50;                        /* ticks */

    /*--- days ---*/

    *day= dt.day-1;                         /* current month days */

    /* previous months days downto january */
    if (dt.mon>1) {                      /* if previous month exists */
        dt.mon--;
        if (dt.mon>2 && adfIsLeap(dt.year))    /* months after a leap february */
            jm[2-1]=29;
        while(dt.mon>0) {
            *day=*day+jm[dt.mon-1];
            dt.mon--;
        }
    }

    /* years days before current year downto 1978 */
    if (dt.year>78) {
        dt.year--;
        while(dt.year>=78) {
            if (adfIsLeap(dt.year))
                *day=*day+366;
            else
                *day=*day+365;
            dt.year--;
        }
    }
}



/*
 * dumpBlock
 *
 * debug function : to dump a block before writing the check its contents
 *
 */
void dumpBlock ( const uint8_t * const buf )
{
  /* Edit PdV */
  /*
	int i, j;

	for(i=0; i<32; i++) {
		printf("%5x ",i*16);
        for (j=0; j<4; j++) {
			printf("%08x ",Long(buf+j*4+i*16));
		}
        printf("    ");
        for (j=0; j<16; j++)
            if (buf[i*16+j]<32 || buf[i*16+j]>127)
                putchar('.');
            else
                putchar(buf[i*16+j]);
        putchar('\n');
    }
   */
}



/*################################################################################*/
