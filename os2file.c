/*
 * os2file.c - OS-specific dir search routines.
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

#define INCL_DOS
#define INCL_DOSERRORS

#include <os2.h>
#include <string.h>
#include <stdlib.h>

#include "osfile.h"

static unsigned long s_flags2os2 (int flags);
static void s_os2date_to_date (struct date_s *pdate, const FDATE *pos2date);
static void s_os2time_to_time (struct time_s *ptime, const FTIME *pos2time);
static int s_ff2fi (struct fileinfo *pfi, const FILEFINDBUF3 *pff, int nameonly);



void* os_findfirst (const char *filespec, int flags, struct fileinfo *pfi, int nameonly)
{
        PHDIR   phdir;
        APIRET  rc;
        ULONG   attr, matches = 1;
        FILEFINDBUF3    ff;
        char    fs[FILENAME_MAX + 1];

        if (!filespec || (strlen (filespec) > FILENAME_MAX))
                return NULL;

        if (!pfi || (NULL == (phdir = malloc (sizeof *phdir))))
                return NULL;

        strcpy (fs, filespec);

        memset (&ff, 0, sizeof ff);
        memset (pfi, 0, sizeof *pfi);

        *phdir = HDIR_CREATE;
        attr = s_flags2os2 (flags);

        if (NO_ERROR != (rc = DosFindFirst (fs, phdir, attr, &ff, sizeof ff,
                                      &matches, FIL_STANDARD))) {
                if (ERROR_NO_MORE_FILES != rc && ERROR_PATH_NOT_FOUND != rc)
                        fprintf (stderr, "Error on 'DosFindFirst' (%s, line %d): %lu\n",
                                __FILE__, __LINE__ - 4, (unsigned long) rc);
                return NULL;
        }

        if (s_ff2fi (pfi, &ff, nameonly)) {
                DosFindClose (*phdir);
                free (phdir);
                phdir = NULL;
        }

        return phdir;
}

int os_findnext (void *handle, struct fileinfo *pfi, int nameonly)
{
        FILEFINDBUF3    ff;
        ULONG   matches = 1;

        memset (&ff, 0, sizeof ff);

        if (NO_ERROR != DosFindNext (*(PHDIR)handle, &ff, sizeof ff, &matches))
                return 2;

        if (!matches)
                return 1;

        if (s_ff2fi (pfi, &ff, nameonly))
                return 3;

        return 0;
}

void os_findclose (void *handle)
{
        DosFindClose (*(PHDIR)handle);
}

static unsigned long s_flags2os2 (int flags)
{
        unsigned long attr = 0;

        attr |= ((flags & OS_MUSTBE_ARCHIVE) ? MUST_HAVE_ARCHIVED : 0);
        attr |= ((flags & OS_MUSTBE_DIR) ? MUST_HAVE_DIRECTORY : 0);
        attr |= ((flags & OS_MUSTBE_SYSTEM) ? MUST_HAVE_SYSTEM : 0);
        attr |= ((flags & OS_MUSTBE_HIDDEN) ? MUST_HAVE_HIDDEN : 0);
        if (flags & OS_MUSTBE_READ) {
                if (!(flags & (OS_MUSTBE_WRITE | OS_MAYBE_WRITE)))
                        attr |= MUST_HAVE_READONLY;
        }

        attr |= ((flags & OS_MAYBE_ARCHIVE) ? FILE_ARCHIVED : 0);
        attr |= ((flags & OS_MAYBE_DIR) ? FILE_DIRECTORY : 0);
        attr |= ((flags & OS_MAYBE_SYSTEM) ? FILE_SYSTEM : 0);
        attr |= ((flags & OS_MAYBE_HIDDEN) ? FILE_HIDDEN : 0);
        if (flags & OS_MAYBE_READ) {
                if (!(flags & OS_MUSTBE_WRITE))
                        attr |= FILE_READONLY;
        }

        return attr;
}


static int s_ff2fi (struct fileinfo *pfi, const FILEFINDBUF3 *pff, int nameonly)
{
        if (strlen (pff->achName) >= sizeof pfi->name) {
                fprintf (stderr,
                         "Serious error in %s, line %d: Filename is too long: '%s'\n"
                         "Maximum allowed is %d characters.",
                         __FILE__, __LINE__, pff->achName, FILENAME_MAX);
                return 1;
        }

        strcpy (pfi->name, pff->achName);
        pfi->namelen = strlen (pfi->name);

        if (!nameonly) {
                s_os2date_to_date (&pfi->create_date, &pff->fdateCreation);
                s_os2date_to_date (&pfi->access_date, &pff->fdateLastAccess);
                s_os2date_to_date (&pfi->write_date, &pff->fdateLastWrite);

                pfi->size = pff->cbFile;
                pfi->alloc = pff->cbFileAlloc;
        }
        return 0;
}


static void s_os2date_to_date (struct date_s *pdate, const FDATE *pos2date)
{
        pdate->year = pos2date->year;
        pdate->month = pos2date->month;
        pdate->day = pos2date->day;
}

static void s_os2time_to_time (struct time_s *ptime, const FTIME *pos2time)
{
        ptime->hours = pos2time->hours;
        ptime->minutes = pos2time->minutes;
        ptime->seconds = 2 * pos2time->twosecs;
}


