/*
 * namelist.c - generic linked list of strings
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

/***************************************
 * Defines / Macros
 ***************************************/


/***************************************
 * Typedefs
 ***************************************/


/***************************************
 * Function Declarations
 ***************************************/

int s_changesize (struct nl_list *plist, size_t newsize);
static size_t s_allocsize (size_t s);
static size_t s_listsize (const struct nl_list *plist);
static size_t s_alignedsize (size_t s);


/***************************************
 * Local Variables
 ***************************************/



/*****************************************************************************/
/*****************************************************************************/


/**************************
 *      nl_addname
 **************************/

int nl_addname (struct nl_list *plist, const char *pname)
{
        struct nl_entry *p;
        size_t          s, lsize;

        if (!(plist && pname))
                return 1;

        s = s_alignedsize (strlen (pname) + sizeof *p);

        if (!plist->pfirst) {
                s = s_allocsize (s);
                if (NULL == (plist->pfirst = malloc (s))) {
                        plist->alloc_size = 0;
                        return 2;
                }
                plist->alloc_size = s;

                p = plist->plast = plist->pfirst;
        }
        else {
                lsize = s_listsize (plist) + s;
                if (plist->alloc_size < lsize) {
                        if (s_changesize (plist, lsize))
                                return 2;
                }
                p = (struct nl_entry*) ((char*)plist->plast +
                      s_alignedsize (strlen (plist->plast->name) + sizeof *p));
                plist->plast->pnext = p;
        }
        strcpy (p->name, pname);
        p->pnext = NULL;
        plist->plast = p;

        return 0;
}



/**************************
 *      nl_findname
 **************************/

struct nl_entry* nl_findname (const struct nl_list *plist, const char *pname)
{
        struct nl_entry   *p;

        if (!(plist && plist->pfirst && pname))
                return NULL;

        p = plist->pfirst;

        while (p) {
                if (!strcmp (p->name, pname))
                        return p;
                p = p->pnext;
        }
        return NULL;
}




/***************************************
 * Local Function Definitions
 ***************************************/



/**************************
 *      s_changesize
 **************************/


int s_changesize (struct nl_list *plist, size_t newsize)
{
        struct nl_entry        *p;

        newsize = s_allocsize (newsize);

        if (plist->pfirst) {
                if (NULL == (p = realloc (plist->pfirst, newsize)))
                        return 2;

                plist->plast = (struct nl_entry*) ((char*)p +
                               ((char*)plist->plast - (char*)plist->pfirst));
                plist->pfirst = p;
                plist->alloc_size = newsize;
        }
        else {
                if (NULL == (plist->pfirst = plist->plast = malloc (newsize))) {
                        plist->alloc_size = 0;
                        return 2;
                }
                plist->alloc_size = newsize;
        }

        return 0;
}



/**************************
 *      s_listsize
 **************************/

static size_t s_listsize (const struct nl_list *plist)
{
        size_t  s;

        if (!(plist && plist->pfirst && plist->plast))
                return 0;

        return s_alignedsize (((char*)plist->plast - (char*)plist->pfirst) +
                         sizeof *plist->plast +
                         strlen (plist->plast->name));
}



/**************************
 *      s_allocsize
 **************************/

static size_t s_allocsize (size_t s)
{
        return (s / BUFSIZ + 1) * BUFSIZ;
}



/**************************
 *      s_alignedsize
 **************************/

static size_t s_alignedsize (size_t s)
{
        size_t  r;

        union {
                long    l;
                double  d;
                char    *p;
        } u;

        r = s % sizeof u;

        if (r)
                return s + sizeof u - r;
        else
                return s;
}


