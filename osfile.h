/*
 * osfile.h - header file for OS-specific functions
 * Copyright (C) 1998 Rupert Weber-Henschel
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
 * along with this program; if not, write to the
 *
 *      Free Software Foundation, Inc.
 *      59 Temple Place - Suite 330
 *      Boston, MA  02111-1307
 *      USA
 *
 * To contact the author, write E-mail to:
 *
 *      rupert.weber@pyhsik.uni-muenchen.de
 *
 * or check the homepage at:
 *
 *      http://www.cip.physik.uni-muenchen.de/~weber/
 *
 *
 * Revision history:
 *      14. Feb 1998 - Initial version
 *
 */


#ifndef FILENAME_MAX
    #include <stdio.h>
#endif

#define OS_MAYBE_ARCHIVE        0x0001
#define OS_MAYBE_DIR            0x0002
#define OS_MAYBE_SYSTEM         0x0004
#define OS_MAYBE_HIDDEN         0x0008
#define OS_MAYBE_READ           0x0010
#define OS_MAYBE_WRITE          0x0020
#define OS_MAYBE_EXEC           0x0040

#define OS_MUSTBE_ARCHIVE       0x0100
#define OS_MUSTBE_DIR           0x0200
#define OS_MUSTBE_SYSTEM        0x0400
#define OS_MUSTBE_HIDDEN        0x0800
#define OS_MUSTBE_READ          0x1000
#define OS_MUSTBE_WRITE         0x2000
#define OS_MUSTBE_EXEC          0x4000

#define OS_NORMAL               (OS_MAYBE_ARCHIVE | OS_MAYBE_READ | \
                                   OS_MAYBE_WRITE | OS_MAYBE_EXEC)
#define OS_NORMAL_HIDDEN        (OS_NORMAL | OS_MAYBE_HIDDEN | OS_MAYBE_SYSTEM)


struct date_s {
        int     year;
        int     month;
        int     day;
};

struct time_s {
        int     hours;
        int     minutes;
        int     seconds;
};

struct fileinfo {
        struct date_s   create_date;
        struct time_s   create_time;
        struct date_s   access_date;
        struct time_s   access_time;
        struct date_s   write_date;
        struct time_s   write_time;
        unsigned long   size;
        unsigned long   alloc;
        size_t          namelen;
        char            name[FILENAME_MAX + 1];
};


void* os_findfirst (const char *filespec, int flags, struct fileinfo *pfi,
                    int nameonly);
int os_findnext (void *handle, struct fileinfo *pfi, int nameonly);
void os_findclose (void *handle);


