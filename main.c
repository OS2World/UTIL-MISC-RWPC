/*
 * main.c - fire up rwpc
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
#include "finddupl.h"


/***************************************
 * Function Declarations
 ***************************************/

static int getcmd (int argc, char **argv, char **ppargv, const char *needarg);
static void usage (void);



/**************************
 *      main
 **************************/

int main (int argc, char **argv)
{
        int     opt;
        char    *arg, *filespec = DEFAULT_FILESPEC, *envvar = DEFAULT_ENV, *path;

        fprintf (stderr, "\n" EXENAME " Ver. " VERSION
                " - Copyright (C) 1998 Rupert Weber-Henschel\n\n"
                "rwpc comes with ABSOLUTELY NO WARRANTY. This is free software,\n"
                "and you are welcome to redistribute it under certain conditions.\n"
                "See the file LICENSE for details.\n\n");

        while (-1 != (opt = getcmd (argc, argv, &arg, "e"))) {
                switch (opt) {
                    case 'e':
                        if (!(envvar = arg)) {
                                usage ();
                                return ERR_INVARG;
                        }
                        break;

                    case 0:
                        if (!(filespec = arg)) {
                                usage ();
                                return ERR_INVARG;
                        }
                        break;

                    default:
                        usage ();
                        return ERR_INVARG;
                }
        }

        if (NULL == (path = getenv (envvar))) {
                fprintf (stderr, "Environment variable '%s' not found!\n",
                         envvar);
                exit (ERR_INVARG);
        }

        fprintf (stderr, "Looking for '%s' in '%s'.\n\n", filespec, envvar);

        return finddupl (path, filespec);
}



/**************************
 *      getcmd
 **************************/

static int getcmd (int argc, char **argv, char **ppargv, const char *needarg)
{
        static int      curr_arg = 1;
        int             opt = -1;
        char            *parg;

        if (curr_arg >= argc)
                return -1;

        parg = argv[curr_arg];

        if ('-' == *parg) {
                opt = *++parg;
                if (*++parg) /* option has an argument */
                        *ppargv = parg;
                else {
                        if (strchr (needarg, opt) && (curr_arg < argc - 1) &&
                            ('-' != *(parg = argv[curr_arg + 1]))) {
                                *ppargv = parg;
                                curr_arg++;
                        }
                        else
                                *ppargv = NULL;
                }
        }
        else {
                opt = 0;
                *ppargv = parg;
        }
        curr_arg++;

        return opt;
}



/**************************
 *      usage
 **************************/

static void usage (void)
{
       fprintf (stderr, "\n Usage:\n\t" EXENAME " [-e <env. variable>] [<filespec>]\n\n");
}


