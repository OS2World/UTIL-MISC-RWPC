/*
 * finddupl.c - find duplicate programs along the PATH
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "namelist.h"
#include "osfile.h"


/***************************************
 * Function Declarations
 ***************************************/

static int is_file_in_dir (const char *filename, const char *dirname);
static int make_fullpath (const char *dirname, const char *filename,
                          char *path, size_t size);
static int check_dir (const struct nl_entry *pdir, const char *filespec,
                      struct nl_list *pfiles);
static void scan_for_file (const struct nl_entry *pdir, const char *filename,
                           struct nl_list *pfiles);

/*****************************************************************************/
/*****************************************************************************/


/**************************
 *      finddupl
 **************************/

int finddupl (const char *path_env, const char *filespec)
{
        char    *paths, *path, *next;
        struct nl_list  nlist, files;
        struct nl_entry *p;
        size_t  dl;

        if (!path_env)
                return ERR_NULLPOINTER;

        memset (&nlist, 0, sizeof nlist);
        memset (&files, 0, sizeof files);

        paths = malloc (strlen (path_env) + 1);
        if (!paths) {
                fprintf (stderr, "Out of memory!\n");
                return ERR_OUTOFMEM;
        }

        strcpy (paths, path_env);

        next = paths;
        dl = strlen (PATH_DELIM);

        while (next) {
                path = next;
                next = strstr (path, PATH_DELIM);
                if (next) {
                        *next = 0;
                        next += dl;
                }
                nl_addname (&nlist, path);
        }

        for (p = nlist.pfirst; p; p = p->pnext) {
                check_dir (p, filespec, &files);
        }

        return 0;
}



/**************************
 *      check_dir
 **************************/

static int check_dir (const struct nl_entry *pdir, const char *filespec,
                      struct nl_list *pfiles)
{
        void    *handle;
        struct  fileinfo        fi;
        char    fullpath[FILENAME_MAX + 1];

        if (!make_fullpath (pdir->name, filespec, fullpath, sizeof fullpath))
                return FALSE;

        handle = os_findfirst (fullpath, OS_NORMAL_HIDDEN, &fi, TRUE);

        while (handle) {
                if (!nl_findname (pfiles, fi.name))
                        scan_for_file (pdir, fi.name, pfiles);

                if (os_findnext (handle, &fi, TRUE)) {
                        os_findclose (handle);
                        handle = NULL;
                }
        }
        return TRUE;
}



/**************************
 *      scan_for_file
 **************************/

static void scan_for_file (const struct nl_entry *pdir, const char *filename,
                           struct nl_list *pfiles)
{
        const struct nl_entry *p;
        int     have_dupl = FALSE;

        for (p = pdir->pnext; p; p = p->pnext) {
                if (is_file_in_dir (filename, p->name)) {
                        if (!have_dupl) {
                                printf ("\n%s appears in the following directories:\n",
                                        filename);
                                have_dupl = TRUE;
                                nl_addname (pfiles, filename);
                                printf ("\t%s\n", pdir->name);
                        }
                        printf ("\t%s\n", p->name);
                }
        }
}



/**************************
 *      make_fullpath
 **************************/

static int make_fullpath (const char *dirname, const char *filename,
                          char *path, size_t size)
{
        size_t  len, dlen;
        int     need_delim = FALSE;

        len = strlen (dirname);
        dlen = strlen (DIR_DELIM);

        if ((len < dlen) || strcmp (dirname + len - dlen, DIR_DELIM)) {
                len += dlen;
                need_delim = TRUE;
        }

        len += strlen (filename);

        if (len + 1 >  size) {
                fprintf (stderr, "Pathname too long:\n Dir: %s\n\nFile: %s\n\n",
                         dirname, filename);
                return FALSE;
        }

        strcpy (path, dirname);
        if (need_delim)
                strcat (path, DIR_DELIM);
        strcat (path, filename);

        return TRUE;
}



/**************************
 *      is_file_in_dir
 **************************/

static int is_file_in_dir (const char *filename, const char *dirname)
{
        char    fullname[FILENAME_MAX + 1];
        void    *handle;
        struct fileinfo fi;

        if (!make_fullpath (dirname, filename, fullname, sizeof fullname))
                return FALSE;

        handle = os_findfirst (fullname, OS_NORMAL_HIDDEN, &fi, TRUE);

        if (handle) {
                os_findclose (handle);
                return TRUE;
        }

        return FALSE;
}



