//******************************************************************************
//*                                 uBeeDisk                                   *
//*                                                                            *
//*     A tool for converting disks/images from one to another with auto       *
//*                   detection options for Microbee disks.                    *
//*                                                                            *
//*                             options module                                 *
//*                                                                            *
//*                       Copyright (C) 2008-2017 uBee                         *
//******************************************************************************
//
//==============================================================================
// ChangeLog (most recent entries are at top)
//==============================================================================
// v4.0.1 - 28 December 2023, Tony Sanchez
// - Change to options_getoptstr() to work around clang strict array bounds check on MacOS
//==============================================================================
// v4.0.0 - 25 January 2017, uBee
// - Added 'dos' 'atari' and 'msx' detection arguments to help() to the
//   --detect option.
// - Added --gapset, --cacher, --cachew and --erase options for remote
//   hardware that supports it.  (i.e Floppyio).
// - Added --pskew, --pskew0 and --pskew1 options.
// - Replaced options_init() with code from ubee512 amd modified to suit.
// - Added functionality for defining variables for scripting use.
// - Added conditional testing options for scripting use.
// - Added --echo and --echoq options.
// - Added separate --datarateip and --datarateop options.
// - Added separate --fmip and --fmop options.
// - Added separate --autorateip and --autorateop options.
// - Changes to --verbose option to take larger values.
// - Removed the --buffer mode option.
// - Added a warning message to options_process() if config file not found.
// - Chnaged xstrverscmp() call to strverscmp() as Win32 version added.
//
// v3.1.0 - 14 November 2016, uBee
// - Added --nofill option to skip 0xe5 fill after the 'finish' point.
// - Added 'IMD' information to the --help option.
//
// v3.0.0 - 15 January 2015, uBee
// - Added --sideoffs option for control over sector side information.
// - Added --fdwa1, --fdwa2 and --fdwa options to provide control over the
//   floppy work-around code.
// - Added --sfmode option to allow --start and --finish to refer to
//   cylinders or tracks.
// - Added --it and --ot options as non ambiguous option names for --itype
//   and --otype as these had been in common use and could have become
//   broken if other --it* or --ot* options were ever added.
// - Added --ifile and --ofile to be consistent with type options names.
// - The --flags option is now deprecated, code and args removed.
// - The --sidedness option is now deprecated, code and args disabled. This
//   option did not actually do anything, a later version may implement it
//   but for now is not thought to be neccessary and should be handled by
//   systems or programs such as cpmtools.
// - Changes to help for --datarate, --format, --start and --finish options.
//
// v2.2.0 - 12 May 2012, uBee
// - Changes to --forceside to allow forcing any sector header side values.
// - Added 'ds8b', 'hs350-80', and 'hs525-80' to --format option help.
//
// v2.1.0 - 8 October 2011, uBee
// - Changes to the --buffering option to take new arguments as this option
//   is now used to change the buffering mode as buffering is always in use.
// - Changes to --skew, --skew-ofs, --verbose and --datarate option help.
// - Added 'applix' and 'fm' information to --detect option help.
// - Added some functions to allow the removal of a lot of repetative
//   coding.  These include: set_int_from_list(), set_int_from_arg().
// - Added c5h0s0_525 format. (not tested)
//
// v2.0.0 - 30 September 2011, uBee
// - Added 'speed' argument to --disk option.
// - Added --buffering option.
// - Added --skew and --skew-ofs otions.
// - Added --scanhead, --ignore-errors and --flags options.
// - Changed the --verbose option to take values higher than 1.
// - Unattended mode is no longer the default required changes to help.
// - Fixed --side1as0 information in help.
//
// v1.6.2 - 12 June 2011, uBee
// - HDD formats 'hd0' and 'hd1' incorrectly documented in --help.  Replaced
//   with 'teac' and 'mscribe' respectively in help for the --format option.
//
// v1.6.0 - 23 July 2010, uBee
// - Added HDD formats 'hd0' and 'hd1' to help for the --format option.
//
// v1.5.0 - 7 July 2010, uBee
// - Revised the help entry for the --retry-l1 option.
//
// v1.4.0 - 26 February 2010, uBee
// - Added --verbose option to set console output reporting level.
// - Added tolower_string() call for --itype, --otype, --format, --datarate,
//   --detect and --disk options.
//
// v1.2.0 - 4 February 2010, uBee
// - Added --unattended option to enable/disable automatic handling of errors.
// - Added --retry-l1 and --retry-l2 retry options.  The existing --retry
//   option is the same as option --retry-l2.
// - Added --unattended-rab option to set number of sector errors allowed
//   before aborting the copy process when the unattended mode is in use.
// - Added --unattended-rps option to set number of retries per sector
//   allowed when the unattended mode is in use.
// - Added --unattended-seek option to set the maximum number of error seeks
//   per disk read allowed when the unattended mode is in use.
// - Added --diskdesc option so that a disk description can be passed.
// - Added --diskdescf option for a disk description contained in a file.
// - Added --force option to stop ubeedisk confirming requested actions.
// - Added --append-error to append the error log (if created) to the 'info'
//   file instead of creating an error file.
//
// v1.1.0 - 10 March 2009, uBee
// - Added --log option to set the level of logging for read errors.
//
// v1.0.0 - 12 December 2008, uBee
// - Created new file.  Parts of the code have been taken from my uBee512
//   emulator project.

//==============================================================================
/*
 *  uBeeDisk - A tool for converting disks/images from one to another with
 *  auto detection options for Microbee disks.
 *  Copyright (C) 2008-2017 uBee
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdint.h>
#include <libdsk.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/utsname.h>
#endif

#include "ubeedisk.h"
#include "options.h"
#include "functions.h"
#include "format.h"
#include "getopt.h"
#include "strverscmp.h"

//==============================================================================
// structures and variables
//==============================================================================
static struct option long_options[] =
{
 {"append-error",    no_argument,       0, OPT_APPENDERR  },
 {"autorate",        required_argument, 0, OPT_AUTORATE   },
 {"autorateip",      required_argument, 0, OPT_AUTORATEIP }, 
 {"autorateop",      required_argument, 0, OPT_AUTORATEOP },
 {"cacher",          required_argument, 0, OPT_CACHER     },
 {"cachew",          required_argument, 0, OPT_CACHEW     },
 {"config",          required_argument, 0, OPT_CONFIG     },
 {"confv",           required_argument, 0, OPT_CONFV      },
 {"count",           required_argument, 0, OPT_COUNT      },
 {"cylinders",       required_argument, 0, OPT_CYLINDERS  }, // option (-c)
 {"datarate",        required_argument, 0, OPT_DATARATE   }, // option (-d)
 {"datarateip",      required_argument, 0, OPT_DATARATEIP },
 {"datarateop",      required_argument, 0, OPT_DATARATEOP },
 {"description",     required_argument, 0, OPT_DESCRIPT   },
 {"detect",          required_argument, 0, OPT_DETECT     },
 {"disk",            required_argument, 0, OPT_DISK       },
 {"diskdesc",        required_argument, 0, OPT_DISKDESC   },
 {"diskdescf",       required_argument, 0, OPT_DISKDESCF  },
 {"echo",            required_argument, 0, OPT_ECHO       },
 {"echoq",           required_argument, 0, OPT_ECHOQ      },
 {"entdesc",         required_argument, 0, OPT_ENTDESC    },
 {"erase",           required_argument, 0, OPT_ERASE      },
 {"fdwa",            required_argument, 0, OPT_FDWA_ALL   },
 {"fdwa1",           required_argument, 0, OPT_FDWA_1     }, 
 {"fdwa2",           required_argument, 0, OPT_FDWA_2     },
 {"flags",           required_argument, 0, OPT_FLAGS      },
 {"finish",          required_argument, 0, OPT_FINISH     },
 {"fm",              required_argument, 0, OPT_FM         },
 {"fmip",            required_argument, 0, OPT_FMIP       },
 {"fmop",            required_argument, 0, OPT_FMOP       },
 {"fmtgap",          required_argument, 0, OPT_FMTGAP     },
 {"force",           no_argument,       0, OPT_FORCE      },
 {"forceside",       required_argument, 0, OPT_FORCESIDE  },
 {"format",          required_argument, 0, OPT_FORMAT     }, // option (-f)
 {"gapset",          required_argument, 0, OPT_GAPSET     },
 {"heads",           required_argument, 0, OPT_HEADS      }, // option (-h)
 {"help",            no_argument,       0, OPT_HELP       },
 {"idstep",          required_argument, 0, OPT_IDSTEP     },
 {"if",              required_argument, 0, OPT_IF         },
 {"ifile",           required_argument, 0, OPT_IF         }, 
 
 // Conditional option parsing
 {"if-egt",         required_argument,  0, OPT_IF_EGT     },
 {"if-elt",         required_argument,  0, OPT_IF_ELT     },
 {"if-eq",          required_argument,  0, OPT_IF_EQ      },
 {"if-gt",          required_argument,  0, OPT_IF_GT      },
 {"if-lt",          required_argument,  0, OPT_IF_LT      },
 {"if-negt",        required_argument,  0, OPT_IF_NEGT    },
 {"if-nelt",        required_argument,  0, OPT_IF_NELT    },
 {"if-neq",         required_argument,  0, OPT_IF_NEQ     }, 
 {"if-ngt",         required_argument,  0, OPT_IF_NGT     },
 {"if-nlt",         required_argument,  0, OPT_IF_NLT     },
 {"if-nset",        required_argument,  0, OPT_IF_NSET    },
 {"if-set",         required_argument,  0, OPT_IF_SET     },
 {"if-system",      required_argument,  0, OPT_IF_SYSTEM  },
 {"if-false",       no_argument,        0, OPT_IF_FALSE   },
 {"if-true",        no_argument,        0, OPT_IF_TRUE    },
 {"if-else",        no_argument,        0, OPT_IF_ELSE    },   
 {"if-end",         no_argument,        0, OPT_IF_END     }, 
 {"if-cmpmode",     required_argument,  0, OPT_IF_CMPMODE },
 
 {"ignore-errors",   required_argument, 0, OPT_IGNORE_ERR },
 {"info",            required_argument, 0, OPT_INFO       },
 {"iside",           required_argument, 0, OPT_ISIDE      },
 {"it",              required_argument, 0, OPT_ITYPE      }, // option (-i)
 {"itype",           required_argument, 0, OPT_ITYPE      }, // option (-i)
 {"lcon",            no_argument,       0, OPT_LCON       },
 {"lconw",           no_argument,       0, OPT_LCONW      },
 {"lcons",           required_argument, 0, OPT_LCONS      },
 {"lformat",         no_argument,       0, OPT_LFORMAT    },
 {"log",             required_argument, 0, OPT_LOG        },
 {"ltype",           no_argument,       0, OPT_LTYPE      },
 {"nofill",          no_argument,       0, OPT_NOFILL     },
 {"noformat",        required_argument, 0, OPT_NOFORMAT   }, // option (-n)
 {"noskip",          required_argument, 0, OPT_NOSKIP     },
 {"nomulti",         required_argument, 0, OPT_NOMULTI    },
 {"odstep",          required_argument, 0, OPT_ODSTEP     },
 {"of",              required_argument, 0, OPT_OF         },
 {"ofile",           required_argument, 0, OPT_OF         }, 
 {"oside",           required_argument, 0, OPT_OSIDE      },
 {"ot",              required_argument, 0, OPT_OTYPE      }, // option (-o)
 {"otype",           required_argument, 0, OPT_OTYPE      }, // option (-o)
 {"pskew",           required_argument, 0, OPT_PSKEW      },
 {"pskew0",          required_argument, 0, OPT_PSKEW0     },
 {"pskew1",          required_argument, 0, OPT_PSKEW1     },
 {"retry-l1",        required_argument, 0, OPT_RETRY_L1   },
 {"retry-l2",        required_argument, 0, OPT_RETRY_L2   },
 {"retry",           required_argument, 0, OPT_RETRY_L2   },
 {"rwgap",           required_argument, 0, OPT_RWGAP      },
 {"scanhead",        required_argument, 0, OPT_SCANHEAD   },
 {"secbase",         required_argument, 0, OPT_SECBASE    },
 {"secbase2s",       required_argument, 0, OPT_SECBASE2S  },
 {"secbase2c",       required_argument, 0, OPT_SECBASE2C  },
 {"secsize",         required_argument, 0, OPT_SECSIZE    },
 {"sectors",         required_argument, 0, OPT_SECTORS    }, // option (-s)
 {"side1as0",        required_argument, 0, OPT_SIDE1AS0   },
 {"sidedness",       required_argument, 0, OPT_SIDEDNESS  },
 {"sideoffs",        required_argument, 0, OPT_SIDEOFFS   }, 
 {"signature",       required_argument, 0, OPT_SIGNATURE  },
 {"skew",            required_argument, 0, OPT_SKEW       },  
 {"skew-ofs",        required_argument, 0, OPT_SKEW_OFS   },  
 {"start",           required_argument, 0, OPT_START      },
 {"sfmode",          required_argument, 0, OPT_SFMODE     },
 {"usage",           no_argument,       0, OPT_HELP       },
 {"unattended",      required_argument, 0, OPT_UNATTENDED },
 {"unattended-rab",  required_argument, 0, OPT_UNATTRAB   },
 {"unattended-rps",  required_argument, 0, OPT_UNATTRPS   },
 {"unattended-seek", required_argument, 0, OPT_UNATTSEEK  },
 {"varset",          required_argument, 0, OPT_VARSET     },
 {"varuset",         required_argument, 0, OPT_VARUSET    },
 {"verbose",         required_argument, 0, OPT_VERBOSE    },
 {"version",         no_argument,       0, OPT_VERSION    }, // option (-v)
 {0,                 0,                 0, 0              }
};

// table to convert short options to a long option number >= 0x100
static short_options_trans_t short_options[] =
{
 {'c', OPT_CYLINDERS},
 {'d', OPT_DATARATE },
 {'f', OPT_FORMAT   },
 {'h', OPT_HEADS    },
 {'i', OPT_ITYPE    },
 {'n', OPT_NOFORMAT },
 {'o', OPT_OTYPE    },
 {'s', OPT_SECTORS  },
 {'v', OPT_VERSION  },
 {  0, 0            }
};

// xgetopt_long stores the long option index here.
int long_index = 0;

char *c_argv[OPTIONS_SIZE];
int c_argc;

char config_file[SSIZE1];
char config_vers[10] = "0";

static char e_optarg[OPTIONS_PARM_SIZE];
static char e_optarg_q[OPTIONS_PARM_SIZE];
static char e_optarg_x[OPTIONS_PARM_SIZE];

static int if_pos;
static int if_state[OPTIONS_MAXCOND];
static int if_cmp_mode;
static int if_state_prev;

#ifdef WIN32
static char win_major_ver[20];
static char win_minor_ver[20];
#else
static struct utsname buf;
static ubd_t ubd;
#endif

static char *ubdenv[OPTIONS_ENV_SIZE];
static int ubdenvc;

#define TRY_MESG "%s: Try `%s --help' or `%s --usage' for more information.\n"

static int int_arg;
static float float_arg;

int exitstatus = 0;

static int list_config_start;
static char *ndefsv[OPTIONS_SIZE];
static int ndefsc;

static char *xargv[OPTIONS_SIZE];
static int xargc;

static FILE *fp;

extern char userhome_confpath[];
extern char userhome[];

extern char *buffer_args[];

extern disk_t disk;
extern dg_opts_t dg_opts;

static char *options_malloc (int size);

//==============================================================================
// Convert console arguments to pointers for each argument found in 's'.
// Memory is allocated with malloc for each argument found.  Doubled quoted
// arguments are preserved and treated as one argument.  The pointer array
// created will be used by the xgetopts_long() function.  A 'ubeedisk'
// argument is inserted for the first argument if no arguments were passed.
//
// This function is used for Windows command line as double quoted arguments
// have no special meaning.
//
//   pass: char *s
// return: void
//==============================================================================
void options_make_pointers (char *s)
{
 char argx[SSIZE1];
 int i;

 // free existing pointers in use
 while (c_argc)
    free(c_argv[--c_argc]);
 
 while ((*s) && (c_argc < OPTIONS_SIZE))
    {
     argx[0] = 0;
     while ((*s) && (*s <= ' '))  // move past white space
        s++;

     i = 0;   

     while ((*s) && (*s != ' ') && (*s != '"'))     
        {
         if (*s)
            {
             argx[i++] = *s;
             s++;
            } 
        }

     if (*s == '"')
        {
         s++;
         while ((*s) && (*s != '"'))
            {
             argx[i++] = *s;
             s++;
            }
         s++;   
        }

     argx[i] = 0;

     if (argx[0])
        {
         c_argv[c_argc] = options_malloc(i+1);

         if (c_argv[c_argc])
            {
             strcpy(c_argv[c_argc], argx);
             c_argc++;
            }
        }
    }

 if (! c_argc)  // insert an argv[0] value of 'ubeedisk' if no args
    {
     strcpy(argx, APPNAME);
     c_argv[c_argc] = options_malloc(strlen(argx)+1);
     if (c_argv[c_argc])
        {
         strcpy(c_argv[c_argc], argx);
         c_argc++;
        } 
    }

#if 0
 for (i = 0; i < c_argc; i++)
    printf("|%s| string len=%d  c_argc=%d\n",
    c_argv[i], strlen(c_argv[i]), c_argc);
 printf("END OF ARGS\n");    
#endif
}

//==============================================================================
// Convert path slash characters to host format
//
//   pass: char *path           path to be converted
// return: void
//==============================================================================
void convert_slash (char *path)
{
 int i = 0;

 while (path[i])
    {
     if (path[i] == SLASHCHAR_OTHER)
        path[i] = SLASHCHAR;
     i++;
    }
}

//==============================================================================
// Open, create or test a file and return the full path.
//
// Mode does not contain 'w'
// -------------------------
// When opening an existing file a test is done to see if the file path
// exists, if not then checks the default location.  If mode is empty ("")
// then only test if the file exists using "rb" as the mode.
//
// Modes:
// "r"   Open a text file for reading
// "a"   Append to a text file
// "rb"  Open a binary file for reading
// "ab"  Append to a binary file
// "r+"  Open a text file for read/write
// "a+"  Open a text file for read/write
// "rb+" Open a binary file for read/write
// "ab+" Open a binary file for read/write    
//
// Mode contains 'w'
// -----------------
// If a file is to be created then the default path is prepended if no
// path slash characters are found.
//
// Modes:
// "w"   Create a text file for writing
// "wb"  Create a binary file for writing
// "w+"  Create a text file for read/write
// "wb+" Create a binary file for read/write
//
// Slash characters are converted to the host system format so either '\' or
// '/' may be used with the same results.
//
//   pass: char *path1          path to be tested
//         char *path2          alternate path prefix to be tried
//         char *path3          the full path
//         char *mode           how to open the file, or "" if test only
//         
// return: *FILE                NULL if no path or file could not be created
//==============================================================================
FILE *open_file (const char *path1, const char *path2, char *path3,
                 const char *mode)
{
 int uses_path1 = 0;
 char pathx[SSIZE1];
 FILE *fp;
 
 strcpy(pathx, path1);

 // convert slash characters to the host's file format if enabled.
 convert_slash(pathx);

 // if a '/', './' or '../' characters are found at the start then treat as
 // the path to be used, the default path (path2) will not be prepened in
 // that case.
 if ((strstr(pathx, "."SLASHCHAR_STR) == pathx) ||
    (strstr(pathx, ".."SLASHCHAR_STR) == pathx) ||
    (strstr(pathx, SLASHCHAR_STR) == pathx))
    uses_path1 = 1;
      
// if Win32 then a ':' character will also be treated as a path to be used.
#ifdef WIN32
 if (strchr(pathx, ':'))
    uses_path1 = 1;
#endif    

 strcpy(path3, pathx);
 
 if (strchr(mode, 'w'))
    // a file is to be created
    {
     if (! uses_path1)
        snprintf(path3, SSIZE1, "%s%s", path2, pathx);
     fp = fopen(path3, mode);
     return fp;
    }

 // test if a file exists, if uses_path1 is true then only the current
 // directory will be checked.
 if (! strlen(mode))
    {
     fp = fopen(path3, "rb");
     if (fp)
        {
         fclose(fp);
         return fp;
        }

     if (uses_path1)
        return fp;

     snprintf(path3, SSIZE1, "%s%s", path2, pathx);
     fp = fopen(path3, "rb");
     if (fp)
        fclose(fp);
     return fp;    
    }

 // open an existing file, if uses_path1 is true then only the current
 // directory will be checked.
 fp = fopen(path3, mode);
 if (fp || uses_path1)
    return fp;

 snprintf(path3, SSIZE1, "%s%s", path2, pathx);
 fp = fopen(path3, mode);
 return fp;
}

//==============================================================================
// Read a text line from a file.
//
// - Leading spaces are removed.
// - Comment and empty lines are not returned.
// - Control characters and space characters are removed from the end of the
//   line.
// - An empty string is returned if an error occurs or the end of the
//   file is reached.
//
//   pass: FILE *fp
//         char *s
//         int size
// return: int                          string length
//==============================================================================
int file_readline (FILE *fp, char *s, int size)
{
 int l;
 int i;
 char *res;
 char temp_str[5000];

 do
    {
     res = fgets(temp_str, sizeof(temp_str)-1, fp);
     if (res)
        {
         // remove leading spaces
         i = 0;
         while ((temp_str[i]) && (temp_str[i] <= ' '))
            i++;
         strncpy(s, &temp_str[i], size);
         s[size-1] = 0;

         // check if this is a commented out line and ignore if is
         if ((s[0] == '#') || (s[0] == ';'))
            {
             s[0] = 0;
             l = 0;
            }
         else
            {
             // otherwise remove trailing control and space characters
             l = strlen(s);
             while ((l) && (s[l-1] <= ' '))
                l--;
             s[l] = 0;   
            }    
        }
     else
        {
         s[0] = 0;
         l = 0;
        } 
    }        
 while ((res) && (! l));
 return l;
}

//==============================================================================
// Unset a uBeeDisk environment variable.
//
// The variable name 'varname' or the variable name and value. i.e
// 'varname=', 'varname=value' may be passed.
//
//   pass: s *char                      uBeeDisk environment variable name
// return: void
//==============================================================================
static void options_ubeedisk_envvar_unset (char *s)
{
 int i;
 int l;
 char *found;
 char *search;

 char temp_str[512];

 // remove '=' character and any trailing string if an '=' character is found.
 if ((search = strchr(s, '=')) == NULL)
    {
     l = strlen(s);
     search = s;
    }
 else
    {
     l = (search - s);
     strncpy(temp_str, s, l);
     temp_str[l] = 0;
     search = temp_str;
    }

 for (i = 0; i < ubdenvc; i++)
    {
     found = strstr(ubdenv[i], search);
     if ((found == ubdenv[i]) && (ubdenv[i][l] == '='))
        {
         free(ubdenv[i]);   // free the memory allocated to it
         ubdenv[i] = NULL;  // incase we only have one pointer
         ubdenvc--;         // one less variable now
         if (ubdenvc)       // if 1 or more than move pointers up
            memmove(&ubdenv[i], &ubdenv[i + 1], sizeof(ubdenv[i]) * (ubdenvc - i));
         return;
        }
    }
}

//==============================================================================
// Set a uBeeDisk environment variable.
//
// A check is first made to see if the variable already exists and if so is
// removed before the new variable is set.
//
// The variable uses the format 'variable_name=variable_value'.  If a
// variable being set does not have a '=' character one will be appended to
// the end of the variable name before being set.
//
//   pass: s *char                      uBeeDisk environment variable name/value
// return: int                          0 if no errors, else -1
//==============================================================================
int options_ubeedisk_envvar_set (char *s)
{
 int l;
 int add_equals = 0;

 // check and remove any variable already defined by this name
 options_ubeedisk_envvar_unset(s);

 l = strlen(s) + 1;

 // variable being searched for must have a '=' character in the string
 if (strchr(s, '=') == NULL)
    add_equals = 1;

 if (ubdenvc < OPTIONS_ENV_SIZE)
    {
     if (! (ubdenv[ubdenvc] = options_malloc(l + add_equals)))
        return -1;
     strcpy(ubdenv[ubdenvc++], s);
     if (add_equals)
        strcat(ubdenv[ubdenvc-1], "=");
     return 0;
    }
 else
    return -1;
}

//==============================================================================
// Return a uBeeDisk environment variable.
//
// Searches a list of variables and returns a pointer to the value if the name
// matches.  A NULL is returned if no matches were found.
//
// The variable uses the format 'variable_name=variable_value'
//
//   pass: s *char                      uBeeDisk environment variable name
// return: char *                       pointer to variable else NULL
//==============================================================================
static char *options_ubeedisk_envvar_get (char *s)
{
 int i;

 char *x;

 for (i = 0; i < ubdenvc; i++)
    {
     x = strstr(ubdenv[i], s);
     if ((x == ubdenv[i]) && (ubdenv[i][strlen(s)] == '='))
        return x + strlen(s) + 1;  // pointer to the string after '='
    }
 return NULL;
}

//==============================================================================
// Compare a uBeeDisk 'string1,string2' using a strvserscmp() or strcmp()
// function depending on the --if-cmpmode option.
//
// The name and value passed uses the format 'string1,string2'
//
//   pass: s *char              "string1,string2" (no leading spaces)
// return: int                  0=equals, -=less, +=greater, -0xf0000=error
//==============================================================================
static int options_ubeedisk_compare (char *s)
{
 char *x;

 char temp_str1[512];
 char temp_str2[512];

 strcpy(temp_str1, s);
 x = strstr(temp_str1, ",");
 if (x)
    {
     *x = 0;
     strcpy(temp_str2, x+1);
     switch (if_cmp_mode)
        {
         case 0 : return strverscmp(temp_str1, temp_str2);
         case 1 : return strcmp(temp_str1, temp_str2);
        }
    }

 return -0xf0000;
}

//==============================================================================
// Extract application, environment and built-in variables.
//
// Application, environment and built-in variables contained in the
// parameter are referenced by using an @ENVVAR@ format in configuration
// files.  Passing environment variables from the command line can use the
// shell's method instead.
//
// Variables may be defined using the --varset option, set as system
// environment variables or use predifined variables. The search order is as
// described so variables may be redefined as the first matching value will
// be used.
//
// Two variables types are returned, e_option_arg contains the @UBEEDISK@ as
// is and e_option_q_arg contains the same but with double quotation marks
// placed around the variable if any spaces are found,  this value is
// intended for passing to Z80 applications where needed.
//
// The built-in pre-defined variables are:
//     UBEEDISK : path to the ubeedisk account.
// UBEE_VERSION : version string of the emulator.
//    UBEE_HOST : host system.
//
// To set uBeeDisk environment variables using options:
// --if-system=win
//    --varset="UBEE_SERIAL=1"
// --if-end
// --if-system=linux
//    --varset="UBEE_SERIAL=/dev/ttyACM0"
// --if-end
//
// To set system environment variables using the host system's shell method:
//
// Windows:
// set ENVVAR=value
//
// bash, sh:
// export ENVVAR=value
//
// csh, tsh:
// setenv ENVVAR value
//
//   pass: char *options_arg
//         char *e_options_arg          
//         char *e_options_q_arg        double quoted variable if any spaces
// return: void
//==============================================================================
static void extract_environment_vars (char *options_arg, char *e_options_arg,
char *e_options_q_arg)
{
#define ENV_NAME_SIZE 1000
 char env[ENV_NAME_SIZE];

 char *ep;

 int o_save = 0;
 int o_index = 0;
 int e_index = 0;
 int e_index_q = 0;
 int v_index;

 e_options_arg[0] = 0;
 e_options_q_arg[0] = 0;

 if (options_arg == NULL)
    return;

 while ((options_arg[o_index]) && (o_index < OPTIONS_PARM_SIZE) &&
 (e_index < OPTIONS_PARM_SIZE))
    {
     if (options_arg[o_index] != '@') // if not start of ENV string
        {
         e_options_arg[e_index++] = options_arg[o_index];
         e_options_q_arg[e_index_q++] = options_arg[o_index];
         e_options_arg[e_index] = 0;
         e_options_q_arg[e_index_q] = 0;
         o_index++;
        }
     else
        {
         v_index = 0;
         env[0] = 0;
         o_index++;
         o_save = o_index;
         while ((options_arg[o_index]) && (options_arg[o_index] != '@') &&
         (o_index < OPTIONS_PARM_SIZE) && (v_index < ENV_NAME_SIZE))
            env[v_index++] = options_arg[o_index++];

         env[v_index] = 0;

         if (options_arg[o_index] == '@')       // if end of ENV string
            {
             o_index++;
             int have_env_var = 0;
#ifdef WIN32
             char s[OPTIONS_PARM_SIZE];
             if (GetEnvironmentVariable(env, s, sizeof(s)) != 0)
                {
                 have_env_var = 1;
                 ep = s;
                }
#else
             ep = getenv(env);
             if (ep)
                have_env_var = 1;
#endif
             else
                {
                 ep = options_ubeedisk_envvar_get(env);
                 if (ep)
                     have_env_var = 1;
                }

             if (have_env_var)
                {
                 strcat(e_options_arg, ep);
                 if (strchr(ep, ' '))  // if contains any spaces
                    {
                     strcat(e_options_q_arg, "\"");  // double quote
                     strcat(e_options_q_arg, ep);
                     strcat(e_options_q_arg, "\"");  // double quote
                    }
                 else
                    strcat(e_options_q_arg, ep);
                }
             e_index = strlen(e_options_arg);
             e_index_q = strlen(e_options_q_arg);
            }
         else
            {
             o_index = o_save;  // go back to index after first '@' char
             e_options_arg[e_index++] = '@';  // treat it as a normal char.
             e_options_q_arg[e_index_q++] = '@';  // treat it as a normal char.
            }
        }
    }

 e_options_arg[e_index] = 0;
 e_options_q_arg[e_index_q] = 0;
}

//==============================================================================
// Options initilisation.
//
// Convert Windows command line arguments to be similar to Unix arguments
// by preserving double quoted arguments.
//
// Set the internal variable values for Unix and Windows systems and some
// other built in variables.
//
//   pass: void
// return: void
//==============================================================================
void options_init (void)
{
 char temp_str[SSIZE1];

#ifdef WIN32
 options_make_pointers(GetCommandLine());

 // get host system version information
 OSVERSIONINFO osvi;

 ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
 osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
 GetVersionEx(&osvi);

 switch (osvi.dwMajorVersion)
    {
     case 3 : strcpy(win_major_ver, "NT3");
              break;
     case 4 : if ((osvi.dwMinorVersion == 0) || (osvi.dwMinorVersion == 10) ||
              (osvi.dwMinorVersion == 90))
                 {
                  strcpy(win_major_ver, "WIN9X_ME");
                  switch (osvi.dwMinorVersion)
                     {
                      case  0 : strcpy(win_minor_ver, "W95");
                                break;
                      case 10 : strcpy(win_minor_ver, "W98");
                                break;
                      case 90 : strcpy(win_minor_ver, "ME");
                                break;
                     }
                  break;
                 }
              if ((osvi.dwMinorVersion == 1) || (osvi.dwMinorVersion == 3))
                 {
                  strcpy(win_major_ver, "NT4");
                  switch (osvi.dwMinorVersion)
                     {
                      case 1 : strcpy(win_minor_ver, "NT4_WS");
                               break;
                      case 3 : strcpy(win_minor_ver, "NT4_SERVER");
                               break;
                     }
                 }
              break;
     case 5 : strcpy(win_major_ver, "NT5");
              switch (osvi.dwMinorVersion)
                 {
                  case 0 : strcpy(win_minor_ver, "W2000");
                           break;
                  case 1 : strcpy(win_minor_ver, "XP");
                           break;
                  case 2 : strcpy(win_minor_ver, "SERVER_2003");
                           break;
                 }
              break;
     case 6 : strcpy(win_major_ver, "NT6");
              switch (osvi.dwMinorVersion)
                 {
                  case 0 : strcpy(win_minor_ver, "VISTA");
                           break;
                  case 1 : strcpy(win_minor_ver, "W7");
                           break;
                  case 2 : strcpy(win_minor_ver, "W8");
                           break;
                  case 3 : strcpy(win_minor_ver, "W8.1");
                           break;
                 }
     case 10 : strcpy(win_major_ver, "NT10");
               switch (osvi.dwMinorVersion)
                  {
                   case 0 : strcpy(win_minor_ver, "W10");
                            break;
                  }
               break;
    }

 options_ubeedisk_envvar_set("UBEE_HOST=WIN");
 snprintf(temp_str, sizeof(temp_str), "UBEE_SYS_MAJOR=%s",
 win_major_ver);
 options_ubeedisk_envvar_set(temp_str);
 snprintf(temp_str, sizeof(temp_str), "UBEE_SYS_MAJOR_VAL=%d",
 (int)osvi.dwMajorVersion);
 options_ubeedisk_envvar_set(temp_str);
 snprintf(temp_str, sizeof(temp_str), "UBEE_SYS_MINOR=%s",
 win_minor_ver);
 options_ubeedisk_envvar_set(temp_str);
 snprintf(temp_str, sizeof(temp_str), "UBEE_SYS_MINOR_VAL=%d",
  (int)osvi.dwMinorVersion);
 options_ubeedisk_envvar_set(temp_str);
#else
 uname(&buf);
 strcpy(ubd.sysname, buf.sysname);
 toupper_string(ubd.sysname, ubd.sysname);

 options_ubeedisk_envvar_set("UBEE_HOST=UNIX");
 options_ubeedisk_envvar_set("UBEE_SYS_MAJOR=UNIX");
 snprintf(temp_str, sizeof(temp_str), "UBEE_SYS_MINOR=%s",
 ubd.sysname);
 options_ubeedisk_envvar_set(temp_str);
#endif

 // set some pre-configured local environment variables
 options_ubeedisk_envvar_set("UBEE_VERSION="APPVER);
}

//==============================================================================
// Options usage.
//
// The last printable character as it appears in this source should be less
// than column position 81.
//
//   pass: void
// return: void
//==============================================================================
static void options_usage (void)
{
 char const usage[] =
"Usage: "APPNAME" [options]\n"
"\n"
"  --append-error          This option is used to append the error log to the\n"
"                          'info' file instead of creating an 'err' file.\n"
"\n"
"  --autorate=x            Test if drive is a 5.25\" HD or 5.25\" DD type if\n"
"                          x=on, if x=off no drive type detection will be used.\n"
"                          When enabled and the drive is a HD type and has less\n"
"                          than 70 tracks specified then double stepping will be\n"
"                          enabled and the data rate changed to DD. This only\n"
"                          affects 'floppy' and 'ntwdm' driver types and devices\n"
"                          that support the AUTORATE option. Default is on.\n"
"  --autorateip=x          Same as --autorate but sets input autorate only.\n"
"  --autorateop=x          Same as --autorate but sets output autorate only.\n"
"\n"
"  --cacher=x              Enable/Disable Track caching for reads. This option\n"
"                          is only supported by some devices (i.e. Floppyio).\n"
"                          x=off to disable, x=on to enable. Default setting\n"
"                          depends on the device.\n"
"  --cachew=x              Enable/Disable Track caching for writes. This option\n"
"                          is only supported by some devices (i.e. Floppyio).\n"
"                          x=off to disable, x=on to enable. Default setting\n"
"                          depends on the device.\n"
"\n"
"  --config=file           Allows an alternative configuration file to be used\n"
"                          or if file='none' then no configuration file will be\n"
"                          used.  This option if used must be the first option\n"
"                          declared on the command line.  The default file used\n"
"                          for configuration is '"APPNAME".ini' and must be\n"
"                          located in the "APPNAME" directory.\n"
"  --confv=v               Intended to be used in ubeedisk.ini scripts as a way\n"
"                          to check for old script versions and warn the user.\n"
"                          'v' is set to match the release version. Set this to\n"
"                          'none' to ignore the version check.\n"
"\n"
"  --count=n               Count value is a number appended to the output file\n"
"                          name base. Using this option makes the copy process\n"
"                          prompt the user for disk changes. i.e. if the output\n"
"                          file name requested is disk.dsk and n=0 then the\n"
"                          name becomes disk000.dsk.  The number is incremented\n"
"                          by one for each iteration.\n"
"\n"
"  --cylinders=n, -c       Set/override the number of cylinders.\n"
"\n"
"  --datarate=x, -d        Set/override the input and output data rates. The\n"
"                          value is based on rates defined by LibDsk. The\n"
"                          allowed values are:\n"
"\n"
"                          hd : data rate for 1.4Mb/1.2Mb disk in 3.5/5.25\"\n"
"                               drive and eight inch drives.\n"
"                          dd : data rate for DD 5.25\" disk in 1.2Mb drive.\n"
"                          sd : data rate for DD disk in 400Kb or 800Kb drive.\n"
"                          ed : data rate for 2.8Mb 3.5\" disk in 3.5\" drive.\n"
"\n"
"  --datarateip=x          Same as --datarate but sets input datarate only.\n"
"  --datarateop=x          Same as --datarate but sets output datarate only.\n"
"\n"
"  --description=str       Set/override the format description.\n"
"\n"
"  --detect=x              Use disk format detection. x determines what disks\n"
"                          should be tested. Currently the supported detection\n"
"                          types are:\n"
"\n"
"                          off    : disable detection.\n"
"                          applix : Applix 1616 disks.\n"
"                          fm     : FM disks (i.e. 8\" disks).\n"
"                          dos    : DOS, Atari ST and MSX-DOS disks.\n"
"                          atari  : DOS Atari ST disks (forces).\n"
"                          msx    : DOS MSX-DOS disks (forces).\n"
"                          mbee   : Microbee disks.\n"
"                          mbee3  : Microbee 3.5\" disks (forces).\n"
"                          mbee5  : Microbee 5.25\" disks (forces).\n"
"\n"
"  --disk=x                Select the required process. Default process is the\n"
"                          'copy' process if not specified. The processes\n"
"                          supported are:\n"
"\n"
"                          copy   : copy standard layout disks.\n"
"                          dump   : not currently implemented.\n"
"                          info   : show disk format information only.\n"
"                          scan   : scan disk tracks for sector ID information.\n"
"                          format : format disk tracks.\n"
"                          speed  : check rotational speed of drive.\n"
"                          clean  : clean disk drive heads.\n"
"\n"
"  --diskdesc=x            Pass a disk description. Repeat this option for as\n"
"                          many lines of text that are required.  Each line may\n"
"                          have up to 100 characters.  This option causes the\n"
"                          prompting for a description to be disabled.\n"
"\n"
"  --diskdescf=fn          Pass the name of a file containing a disk\n"
"                          description.  Each line may have up to 100\n"
"                          characters. This option causes the prompting for a\n"
"                          description to be disabled.\n"
"\n"
"  --echo=x                Echo a string to stdout. The string may also contain\n"
"                          an environment variable.\n"
"  --echoq=x               Same as --echo option but echoes a quoted version of\n"
"                          the environment variable if any spaces are found.\n"
"\n"
"  --entdesc=x             Prompt operator to enter a disk description if x=on\n"
"                          or bypass if x=off when using the copy process.\n"
"                          Default is on.\n"
"\n"
"  --erase=n               Set track erasure value to be used if formatting.\n"
"                          This option is only supported by some devices\n"
"                          (i.e. Floppyio). This forces a low level format of\n"
"                          the entire track to the same value.  The 'n' value\n"
"                          should not be a special controller value.\n"
"\n"
"  --fdwa1=x               This option may be used to enable/disable some PC\n"
"                          only floppy disk access work-around #1 code. If\n"
"                          enabled the input drive is closed then reopened at\n"
"                          certain points in the program. Default is enabled.\n"
"\n"
"  --fdwa2=x               This option may be used to enable/disable some floppy\n"
"                          disk access work-around #2 code. If enabled Track #0\n"
"                          is seeked followed by closing/reopening the input\n"
"                          drive at certain points in the program. Default is\n"
"                          enabled.\n"
"\n"
"  --fdwa=x                Enables/disables both the fdwa1 and fdwa2 settings.\n"
"\n"
"  --flags=x               This option has been deprecated.\n"
"\n"
"  --finish=n              Set/override the finish cylinder/track for processes.\n"
"                          Default value is set to the highest value found by\n"
"                          --detect, --format or --cylinder options.  Tracks\n"
"                          may also be specified by using --sfmode.\n"
"\n"
"  --fm=n                  Set/override the use of FM mode.  n=1 if the disk\n"
"                          uses an FM format or n=0 if MFM.\n"
"  --fmip=n                Same as --fm but sets input recording mode only.\n"
"  --fmop=n                Same as --fm but sets output recording mode only.\n"
"\n"
"  --fmtgap=n              Set/override the LibDsk geometry format gap value.\n"
"\n"
"  --force                 Use this option to stop confirmation of any requested\n"
"                          actions. i.e. when formatting a disk.\n"
"\n"
"  --forceside=x           Forces formatting the output sector header ID side\n"
"                          value according to one of the following parameters:\n"
"\n"
"                          off : use the source header values if possible (def).\n"
"                          on  : use the physical head value for side.\n"
"                          00  : use value 0 for head 0 and value 0 for head 1.\n"
"                          01  : use value 0 for head 0 and value 1 for head 1.\n"
"                          10  : use value 1 for head 0 and value 0 for head 1.\n"
"                          11  : use value 1 for head 0 and value 1 for head 1.\n"
"\n"
"  --format=x, -f          Specify the disk format to use. The value is first\n"
"                          checked against built in values and if no matches\n"
"                          are found then against values in LibDsk and\n"
"                          the 'libdskrc' file. Use the --lformat option to see\n"
"                          a list of the available formats.\n"
"\n"
"  --gapset=pe:v,[pe:v...] Set number of format GAP/SYNC bytes for format gaps.\n"
"                          See the README file for usage.\n"
"\n"
"  --heads=n, -h           Set/override the number of heads.\n"
"\n"
"  --help                  Send this help information to stdout.\n"
"\n"
"  --idstep=x              Determine if the input drive should use double\n"
"                          stepping.  x=on to enable, x=off to disable.\n"
"                          Default is off. (auto detection may set to on)\n"
"\n"
"  --if=name, --ifile      Input file name.  This may be a floppy device or\n"
"                          name of a disk image.  On Windows and Unices systems\n"
"                          floppy devices 'A:', 'B:', etc. and on Unices\n"
"                          '/dev/fd0', '/dev/fd1', etc. may be used. Default\n"
"                          for input file will be 'A:' or '/dev/fd0'.\n"                          
"\n"
"  --if-*                  Conditional processing options for scripting use.\n"
"                          If any of the conditionals returns a true result\n"
"                          then option processing is enabled, a false result\n"
"                          turns processing off until a true condition is met.\n"
"                          See the README file for all available options.\n"
"\n"
"  --ignore-errors=x       When copying and Interactive mode is in context all\n"
"                          copy errors will be ignored if this option is\n"
"                          enabled. x=on to enable, x=off to disable. This\n"
"                          action may also be enabled on a per disk copy from\n"
"                          the Interactive menu. Default is off.\n"
"\n"
"  --info=x                Determine if an 'info' file should be created. The\n"
"                          'info' file uses the output name with '.info'\n"
"                          appended. x=on to enable, x=off to disable. Default\n"
"                          is on.  The 'info' file contains very important\n"
"                          information about the disk image and should always\n"
"                          be enabled and the file kept.\n"
"\n"
"  --iside=n               Force side n of input. n may be 0 or 1.\n"
"\n"
"  --it=x, --itype, -i     Set/override the input driver type.  If no type is\n"
"                          given the input file name will be used to determine\n"
"                          the appropriate type.  The following types are\n"
"                          supported by the LibDsk library, see the LibDsk\n"
"                          documentation for further details: dsk, edsk, raw,\n"
"                          logical, floppy, ntwdm, myz80, cfi, qm, teledisk,\n"
"                          imd, nanowasp, rcpmfs, remote.\n"
"\n"
"                          If 'fd' is specified for x then this will be\n"
"                          translated to the host system's floppy driver. On\n"
"                          Windows this is 'ntwdm' and Unices is the 'floppy'\n"
"                          driver.  This is useful for making scripts portable.\n"
"\n"
"  --lcon                  List the [section] names found in the configuration\n"
"                          file.\n"
"  --lconw                 Same as --lcon option except uses a wide format.\n"
"  --lcons=n               Sets the list start point for --lcon and --lconw\n"
"                          options. Default value is 1.\n"
"\n"
"  --lformat               Lists all the LibDsk built-in and external disk\n"
"                          formats that are available.\n"
"\n"
"  --log=n                 Logging level, set the log level when 'info' files\n"
"                          are created to one of the following. Default is 1.\n"
"\n"
"                          0 : No read errors are logged.\n"
"                          1 : Read errors are logged only after the retry\n"
"                              count expires.\n"
"                          2 : All read errors are logged. Very useful for\n"
"                              checking for floppy drive and HW problems.\n"
"\n"
"  --ltype                 Lists all the LibDsk driver types that are available.\n"
"\n"
"  --nofill                Normally all tracks from 'start' to 'finish' are\n"
"                          written with actual data and the remainder filled\n"
"                          in with 0xe5 bytes for the format concerned,  this\n"
"                          option forces completion at the 'finish' point\n"
"                          selected with no end fill.\n"
"\n"
"  --noformat=n, -n        Set n=1 to prevent or n=0 to allow formatting of\n"
"                          the output drive/image.  Default is 0.\n"
"\n"
"  --nomulti=n             Set/Override no multi-track mode. Set n=1 to prevent\n"
"                          or n=0 to allow multi-track mode.\n"
"\n"
"  --noskip=n              Set/Override no skipping of deleted data. Set n=1\n"
"                          to prevent or n=0 to allow skipping.\n"
"\n"
"  --odstep=x              Determine if the output drive should use double\n"
"                          stepping.  x=on to enable, x=off to disable.\n"
"                          Default is off. (auto detection may set to on)\n"
"\n"
"  --of=name, --ofile      Output file name.  This may be a floppy device or\n"
"                          name of a disk image.  On Windows and Unices systems\n"
"                          floppy devices 'A:', 'B:', etc. and on Unices\n"
"                          '/dev/fd0', '/dev/fd1', etc. may be used.\n"
"                          Output names support substitution '@' characters\n"
"                          (see README for full details).\n"
"\n"
"  --oside=n               Force side n of output. n may be 0 or 1.\n"
"\n"
"  --ot=x, --otype, -o     Set/override the output driver type.  If no type is\n"
"                          given the output file name will be used to determine\n"
"                          the appropriate type.  The following types are\n"
"                          supported by the LibDsk library, see the LibDsk\n"
"                          documentation for further details: dsk, edsk, raw,\n"
"                          logical, floppy, ntwdm, myz80, cfi, qm, teledisk,\n"
"                          imd, nanowasp, rcpmfs, remote.\n"
"\n"
"                          If 'fd' is specified for x then this will be\n"
"                          translated to the host system's floppy driver. On\n"
"                          Windows this is 'ntwdm' and Unices is the 'floppy'\n"
"                          driver.  This is useful for making scripts portable.\n"
"\n"
"  --pskew=n,n,n...        Set physical sector skewing for track formatting. A\n"
"                          maximum of 256 values are allowed. This will be used\n"
"                          by side 0 and side 1 of the disk.\n"
"  --pskew0=n,n,n...       As for --pskew but values applies to side 0 only.\n"
"  --pskew1=n,n,n...       As for --pskew but values applies to side 1 only.\n"
"\n"
"  --retry-l1=n            Set the number of read tries at the lowest level\n"
"                          before level 2 comes into play. Default value is 30\n"
"                          for a 'copy' and 5 for a 'scan/speed' command.  1 is\n"
"                          the default value if the input type is 'remote'.\n"
"  --retry-l2=n, --retry=n Set the 2nd level of read tries before the 3rd level\n"
"                          comes into play.  The 3rd and final level is an\n"
"                          unattended or interactive mode. Default value is 3.\n"
"\n"
"  --rwgap=n               Set/override the read/write sector gap value.\n"
"\n"
"  --scanhead=s[,f]        Set the head range to be scanned when using the\n"
"                          --disk=scan option, 's' is the head to start with,\n"
"                          'f' is an optional finish head otherwise finish will\n"
"                          be the same as 's'. Default values are 0,1.\n"
"\n"
"  --secbase=n             Set/override the sector base number.\n"
"\n"
"  --secbase2s=n           Set/override the sector base2 number.\n"
"\n"
"  --secbase2c=n           Set/override the sector base2 cylinder start number.\n"
"\n"
"  --secsize=n             Set/override the sector size. Must be >= 128 and\n"
"                          a power of 2.\n"
"\n"
"  --sectors=n, -s         Set/override the number of sectors per track\n"
"\n"
"  --sfmode=x              Set the mode for --start and --finish values. x='t'\n"
"                          for tracks or x='c' for cylinder mode.  By default\n"
"                          the mode is cylinders.  When set to track mode the\n"
"                          start and finish values can have finer control.\n"
"\n"
"  --side1as0=n            Set/override the physical properties of side 1\n"
"                          sector header ID side information. n=1 indicates the\n"
"                          header uses 0 instead of 1. n=0 indicates the side\n"
"                          information matches the head 1 number.\n"
"\n"
#if 0
"  --sidedness=x           Set/override the sidedness of the disk. The allowed\n"
"                          values are:\n"
"\n"
"                          alt     : Track n is cylinder (n/heads) head\n"
"                                    (n\%heads)\n"
"                          outback : Tracks go (head 0) 0,1,2,3,...37,38,39,\n"
"                                    then (head 1) 39,38,37,...,2,1,0\n"
"                          outout  : Tracks go (head 0) 0,1,2,3,...37,38,39,\n"
"                                    then (head 1) 0,1,2,3,...37,38,39\n"
"\n"
#else
"  --sidedness=x           This option has been deprecated.\n"
#endif
"  --sideoffs=n            Set side offset value used in sector headers. Most\n"
"                          disks use 0 but some may use an offset for other\n"
"                          purposes such as a double sided disk flag. The\n"
"                          default value is as defined in the format.\n"
"\n"
"  --signature=name        The archiver's signature can appear in the 'info'\n"
"                          file by setting a name.\n"
"\n"
"  --skew=n                Set/override the skew value used by the track read\n"
"                          process. This value when correctly set can greatly\n"
"                          improve the disk reading speed during the copy\n"
"                          process. The default skew is set to 2.\n"
"  --skew-ofs=n            Set/override the first physical sector number read in\n"
"                          a track read process. The first physical sector will\n"
"                          be this value added to the sector base value at the\n"
"                          time of reading, if the sector base number is 1\n"
"                          (--secbase=1) and n=0 the first sector read will be\n"
"                          1. The default value is 1.\n"
"\n"
"  --start=n               Set/override the start cylinder/track for processes.\n"
"                          Default this value is set to 0 cylinders but tracks\n"
"                          may also be specified by using --sfmode.\n"
"\n"
"  --unattended=x          Use this option to enable/disable automated error\n"
"                          handling. x=on to enable, x=off to disable.\n"
"                          Default is off.\n"
"  --unattended-rab=n      Set number of sector errors allowed before aborting\n"
"                          the copy process when the unattended mode is in use.\n"
"                          Default value is 300.\n"
"  --unattended-rps=n      Set number of retries per sector allowed when the\n"
"                          unattended mode is in use.  Default value is 2.\n"
"  --unattended-seek=n     Set the maximum number of error seeks per disk read\n"
"                          allowed when the unattended mode is in use. After\n"
"                          this no more seeks to track 0 will occur between\n"
"                          read errors. Default value is 10.\n"
"\n"
"  --verbose=n             Set the reporting level. A higher level includes\n"
"                          reporting of any lower levels. The verbose reporting\n"
"                          levels are:\n"
"\n"
"                          0  : Only error messages, interaction prompts and\n"
"                               processes intended to convey general information\n"
"                               will be output.\n"
"                          1  : Normal program output reporting (default).\n"
"                          >1 : Additional reporting levels.\n"
"\n"
"  --version, -v           Output the program version number to stdout.\n"
"\n"
"If you have any new feature suggestions, bug reports, etc. then post a new\n"
"topic at www.microbee-mspp.org.au\n";
 printf("%s", usage);
}

//==============================================================================
// Print parameter error message and set exit status.
//
//   pass: void
// return: void
//==============================================================================
static void param_error_mesg (void)
{
#define PARMERR_MESG APPNAME": option `--%s' argument of '%s' is not permitted\n"
 printf(PARMERR_MESG, (char *)long_options[long_index].name, e_optarg);
 exitstatus = 1;
}

//==============================================================================
// Set a single integer value from a passed arguments list.
//
//   pass: int *value                   pointer to the value to be set
//         char *use_args               array of arguments allowed
// return: int                          -1 if error else value
//==============================================================================
int set_int_from_list (int *value, char *use_args[])
{
 int res;

 res = string_search(use_args, e_optarg);

 if (res == -1)
    {
     printf(PARMERR_MESG, (char *)long_options[long_index].name, e_optarg);
     exitstatus = 1;
    }
 else
    *value = res;

 return res;
}

//==============================================================================
// Set a single integer value if between limits.
//
//   pass: int *value                   pointer to the value to be set
//         int min                      minimum value allowed
//         int max                      maximum value allowed
// return: int                          -1 if error else 0
//==============================================================================
int set_int_from_arg (int *value, int min, int max)
{
 if ((int_arg >= min) && (int_arg <= max))
    {
     *value = int_arg;
     return 0;
    }

 printf(PARMERR_MESG, (char *)long_options[long_index].name, e_optarg);
 exitstatus = 1;
 return -1;
}

//==============================================================================
// Process options.
//
// If any non-options are found these are processed last (see man getopt). 
// If any options processed causes exiting the options module before those
// non-options are reached then no error will be seen.
//
// In the "short options" string passed in xgetopt() a single ':' following
// an option letter indicates an argument for the optionis required.  Two
// '::' indicates an optional argument and no colon indicates no option. 
// The optarg string will be empty if an option is set to 'optional
// argument' type and no argument was sepecified.
//
//
//   pass: int argc                     number of arguments
//         char *argv[]                 pointer to pointer to arguments
// return: void
//==============================================================================
void options_getopt (int argc, char *argv[])
{
 char *offon_args[] =
 {
  "off",
  "on",
  ""
 };

 char *disk_args[] =
 {
  "copy",
  "dump",  
  "info",
  "scan",
  "format",
  "speed",
  "clean",
  ""
 };

 char *datarate_args[] =
 {
  "hd",
  "dd",
  "sd",
  "ed",
  ""
 };

#if 0
char *sidedness_args[] =
 {
  "alt",
  "outback",
  "outout",
  ""
 };
#endif 

 char *forceside_args[] =
 {
  "off",
  "on",
  "00",
  "01",
  "10",
  "11",
  ""
 };

 char *sfmode_args[] =
 {
  "c",
  "t",
  ""
 };

 int i;
 int c;
 int x;
 int short_option;

 // the xgetopt variables must be reset each time this function is re-entered.
 xgetopt_init();

#if 0
 // if no parameters then give a hint on what to do
 if (argc <= 1)
   {
    printf(TRY_MESG, argv[0], argv[0], argv[0]);
    exitstatus = 1;
    return;
   }
#endif    

 while (! exitstatus)
    {
     c = xgetopt_long(argc, argv, "ab:c:d:f:h:i:l:no:sv",
     long_options, &long_index);

     // print any error messages generated by xgetopt_long()
     if (opterr_msg[0])
        {
         exitstatus = 1;
         printf("%s", opterr_msg);
         printf(TRY_MESG, argv[0], argv[0], argv[0]);
        }

     // detect the end of the options
     if (c == -1)
        break;

     // translate short options to a long option number
     short_option = (c < 0x100) * c;
     if (short_option)
        {
         i = 0;
         while ((short_options[i].option) && (short_options[i].option != c))
            i++;
         if (short_options[i].option == c)
            c = short_options[i].longno;
        }
        
     // don't process options if currently off (except for --if-x options)
     if (((c < OPT_GROUP_CONDITIONAL) || (c > (OPT_GROUP_CONDITIONAL + 0xff)))
     && ((c != '?') && (c != ':')))
        {
         if (if_pos >= OPTIONS_MAXCOND)
            {
             printf(APPNAME": all %d levels of conditionals used up!\n",
             OPTIONS_MAXCOND);
             exitstatus = -1;
             c = 0;
            }
         if ((if_state[if_pos]) == 0)
            c = 0;
        }

     if_state_prev = if_state[if_pos];

     if (c)
        {
         // do integer and floating point conversions
         if (optarg != NULL)
            {
             int_arg = -1;
             if ((strchr(optarg, 'x') == NULL) &&
                (strchr(optarg, 'X') == NULL))
                sscanf(optarg, "%d", &int_arg);
             else   
                sscanf(optarg, "%x", &int_arg);         
             float_arg = -1.0;
             sscanf(optarg, "%f", &float_arg);
            }
            
         // limit all arguments to maximum 1000 characters including NULL
         if (optarg)  // may be NULL if no argument
            {
             // extract environment variables from the argument
             extract_environment_vars(optarg, e_optarg, e_optarg_q);
             toupper_string(e_optarg_x, e_optarg);
#if 0
             xprintf("optarg=%s, e_optarg=%s e_optarg_q=%s\n",
             optarg, e_optarg, e_optarg_q);
#endif
            } 
         else
            e_optarg[0] = 0;

         switch (c)
            {
             case OPT_APPENDERR :
                disk.append_error = 1;
                break;
             case OPT_AUTORATE :
                set_int_from_list(&disk.iautorate, offon_args);
                set_int_from_list(&disk.oautorate, offon_args);
                break;
             case OPT_AUTORATEIP :
                set_int_from_list(&disk.iautorate, offon_args);
                break;
             case OPT_AUTORATEOP :
                set_int_from_list(&disk.oautorate, offon_args);
                break;
             case OPT_CACHER :
                set_int_from_list(&disk.cacher, offon_args);             
                break;
             case OPT_CACHEW :
                set_int_from_list(&disk.cachew, offon_args);
                break;
             case OPT_CONFIG :
                strncpy(config_file, e_optarg, sizeof(config_file));
                config_file[sizeof(config_file)-1] = 0;
                break;
             case OPT_CONFV :
                strncpy(config_vers, e_optarg, sizeof(config_vers));
                config_file[sizeof(config_vers)-1] = 0;
                toupper_string(config_vers, config_vers);
                break;
             case OPT_COUNT :
                set_int_from_arg(&disk.count, 0, 1000000);
                break;
             case OPT_CYLINDERS :
                set_int_from_arg(&dg_opts.cylinders, 0, 1000000);
                break;
             case OPT_DATARATE :
                tolower_string(e_optarg, e_optarg);
                set_int_from_list(&dg_opts.idatarate, datarate_args);
                set_int_from_list(&dg_opts.odatarate, datarate_args);
                break;
             case OPT_DATARATEIP :
                tolower_string(e_optarg, e_optarg);
                set_int_from_list(&dg_opts.idatarate, datarate_args);
                break;
             case OPT_DATARATEOP :
                tolower_string(e_optarg, e_optarg);
                set_int_from_list(&dg_opts.odatarate, datarate_args);
                break;
             case OPT_DESCRIPT :
                strcpy(dg_opts.format_name, e_optarg);
                break;
             case OPT_DETECT :
                tolower_string(e_optarg, e_optarg);
                x = format_detect_select(e_optarg);
                if (x == -1) 
                   param_error_mesg();
                break;             
             case OPT_DISK :
                tolower_string(e_optarg, e_optarg);
                set_int_from_list(&disk.disk, disk_args);
                break;
             case OPT_DISKDESC :
                process_disk_description(optarg);
                break;
             case OPT_DISKDESCF :
                if (file_disk_description(optarg))
                   {
                    printf(APPNAME
                           ": File specified by --diskdescf not found.\n");
                    exitstatus = 1;
                   } 
                break;
             case OPT_ECHO :
                printf("%s\n", e_optarg);
                break;
             case OPT_ECHOQ :
                printf("%s\n", e_optarg_q);
                break;
             case OPT_ENTDESC :
                set_int_from_list(&disk.enter_desc, offon_args);
                break;
             case OPT_ERASE :   
                set_int_from_arg(&disk.erase, 0, 255);
                break;
             case OPT_FDWA_ALL :
                set_int_from_list(&disk.fd_workaround1, offon_args);
                disk.fd_workaround2 = disk.fd_workaround1;
                break;
             case OPT_FDWA_1 :
                set_int_from_list(&disk.fd_workaround1, offon_args);
                break;
             case OPT_FDWA_2 :
                set_int_from_list(&disk.fd_workaround2, offon_args);
                break;
             case OPT_FLAGS :
                printf(APPNAME": --flags option has been deprecated.\n");
                exitstatus = 1;
                break;
             case OPT_FINISH :
                set_int_from_arg(&disk.finish, 0, 1000000);
                break;
             case OPT_FM :
                set_int_from_arg(&dg_opts.ifm, 0, 1);
                set_int_from_arg(&dg_opts.ofm, 0, 1);
                break;
             case OPT_FMIP :
                set_int_from_arg(&dg_opts.ifm, 0, 1);
                break;
             case OPT_FMOP :
                set_int_from_arg(&dg_opts.ofm, 0, 1);
                break;
             case OPT_FORCE :
                disk.force = 1;
                break;
             case OPT_FORCESIDE :
                set_int_from_list(&disk.forceside, forceside_args);
                break;
             case OPT_FORMAT :
                strcpy(disk.format, e_optarg);
                tolower_string(disk.format, disk.format);
                break;
             case OPT_FMTGAP :
                set_int_from_arg(&dg_opts.fmtgap, 0, 1000000);
                break;
             case OPT_GAPSET :
                if (get_gap_colon_arguments(e_optarg, disk.gap_set, 4, 0x3fff) == -1)
                   param_error_mesg();
                break;
             case OPT_HEADS :
                set_int_from_arg(&dg_opts.heads, 0, 1000000);
                break;
             case OPT_HELP :
                options_usage();
                exitstatus = 1;
                break;
             case OPT_IDSTEP :
                set_int_from_list(&disk.idstep, offon_args);
                break;
             case OPT_IF :
                strcpy(disk.ifile, e_optarg);
                break;

             // Process options: Conditional option parsing.
             case OPT_IF_EGT :
                x = options_ubeedisk_compare(e_optarg);
                if (x == -0xf0000)
                   param_error_mesg();
                else
                   if_state[++if_pos] = (x >= 0) & if_state_prev;
                break;
             case OPT_IF_ELT :
                x = options_ubeedisk_compare(e_optarg);
                if (x == -0xf0000)
                   param_error_mesg();
                else
                   if_state[++if_pos] = (x <= 0) & if_state_prev;
                break;
             case OPT_IF_EQ :
                x = options_ubeedisk_compare(e_optarg);
                if (x == -0xf0000)
                   param_error_mesg();
                else
                   if_state[++if_pos] = (x == 0) & if_state_prev;
                break;
             case OPT_IF_GT :
                x = options_ubeedisk_compare(e_optarg);
                if (x == -0xf0000)
                   param_error_mesg();
                else
                   if_state[++if_pos] = (x > 0) & if_state_prev;
                break;
             case OPT_IF_LT :
                x = options_ubeedisk_compare(e_optarg);
                if (x == -0xf0000)
                   param_error_mesg();
                else
                   if_state[++if_pos] = (x < 0) & if_state_prev;
                break;
             case OPT_IF_NEGT :
                x = options_ubeedisk_compare(e_optarg);
                if (x == -0xf0000)
                   param_error_mesg();
                else
                   if_state[++if_pos] = (x < 0) & if_state_prev;
                break;
             case OPT_IF_NELT :
                x = options_ubeedisk_compare(e_optarg);
                if (x == -0xf0000)
                   param_error_mesg();
                else
                   if_state[++if_pos] = (x > 0) & if_state_prev;
                break;
             case OPT_IF_NEQ :
                x = options_ubeedisk_compare(e_optarg);
                if (x == -0xf0000)
                   param_error_mesg();
                else
                   if_state[++if_pos] = (x != 0) & if_state_prev;
                break;
             case OPT_IF_NGT :
                x = options_ubeedisk_compare(e_optarg);
                if (x == -0xf0000)
                   param_error_mesg();
                else
                   if_state[++if_pos] = (x <= 0) & if_state_prev;
                break;
             case OPT_IF_NLT :
                x = options_ubeedisk_compare(e_optarg);
                if (x == -0xf0000)
                   param_error_mesg();
                else
                   if_state[++if_pos] = (x >= 0) & if_state_prev;
                break;
             case OPT_IF_NSET :
                if_state[++if_pos] =
                (options_ubeedisk_envvar_get(e_optarg) == NULL) & if_state_prev;
                break;
             case OPT_IF_SET :
                if_state[++if_pos] =
                (options_ubeedisk_envvar_get(e_optarg) != NULL) & if_state_prev;
                break;
             case OPT_IF_SYSTEM :
#ifdef WIN32
                x = ((strcmp(e_optarg_x, "WIN") == 0) ||
                (strcmp(e_optarg_x, win_major_ver) == 0) ||
                (strcmp(e_optarg_x, win_minor_ver) == 0));
#else
                x = ((strcmp(e_optarg_x, "UNIX") == 0) ||
                (strcmp(e_optarg_x, ubd.sysname) == 0));
#endif
                if_state[++if_pos] = x & if_state_prev;
                break;
             case OPT_IF_FALSE :
                if_state[++if_pos] = 0 & if_state_prev;
                break;
             case OPT_IF_TRUE :
                if_state[++if_pos] = 1 & if_state_prev;
                break;
             case OPT_IF_ELSE :
                if (if_state[if_pos - 1])
                   if_state[if_pos] = ! if_state[if_pos];
                break;
             case OPT_IF_END :
                if (if_pos)
                   if_pos--;
                break;
             case OPT_IF_CMPMODE :
                set_int_from_arg(&if_cmp_mode, 0, 1);
                break;

             // other options
             case OPT_IGNORE_ERR :
                set_int_from_list(&disk.ignore_errors, offon_args);
                break;
             case OPT_ISIDE :
                set_int_from_arg(&disk.iside, 0, 1);
                break;
             case OPT_ITYPE :
                strcpy(disk.itype, e_optarg);
                tolower_string(disk.itype, disk.itype);
                break;
             case OPT_LCON :
                for (i = list_config_start; i < ndefsc; i++)
                   printf("%s\n", ndefsv[i]);
                exitstatus = -1;
                break;
             case OPT_LCONW :
                i = list_config_start;
                x = 0;
                while (i < ndefsc)
                   {
                    printf("%-16s", ndefsv[i++]);
                    x++;
                    if ((x % 5) == 0)
                       printf("\n");
                   }    
                if ((x % 5) != 0)
                   printf("\n");
                exitstatus = -1;
                break;
             case OPT_LCONS :
                if (set_int_from_arg(&list_config_start, 1, ndefsc) == -1)
                   break;
                list_config_start -= 1;
                break;
             case OPT_LFORMAT :
                format_list_formats();
                exitstatus = 1;
                break;
             case OPT_LOG :
                set_int_from_arg(&disk.log, 0, 2);
                break;
             case OPT_LTYPE :
                format_list_types();
                exitstatus = 1;
                break;
             case OPT_INFO :
                set_int_from_list(&disk.info_file, offon_args);
                break;
             case OPT_NOFILL :
                disk.nofill = 1;
                break;
             case OPT_NOFORMAT :
                set_int_from_arg(&disk.noformat, 0, 1);
                break;
             case OPT_NOMULTI :
                set_int_from_arg(&dg_opts.nomulti, 0, 1);
                break;
             case OPT_NOSKIP :
                set_int_from_arg(&dg_opts.noskip, 0, 1);
                break;
             case OPT_ODSTEP :
                set_int_from_list(&disk.odstep, offon_args);
                break;
             case OPT_OF :
                strcpy(disk.ofile, e_optarg);
                break;
             case OPT_OSIDE :
                set_int_from_arg(&disk.oside, 0, 1);
                break;
             case OPT_OTYPE :
                strcpy(disk.otype, e_optarg);
                tolower_string(disk.otype, disk.otype);
                break;
             case OPT_PSKEW :
                if (get_int_arguments(e_optarg, disk.pskew0,
                PSKEW_SIZE, 255) != -1)
                   {
                    memcpy(disk.pskew1, disk.pskew0, sizeof(disk.pskew1));
                    disk.pskew0_opt = 1;
                    disk.pskew1_opt = 1;                   
                    break;
                   }
                param_error_mesg();
                break;
             case OPT_PSKEW0 :
                if (get_int_arguments(e_optarg, disk.pskew0,
                PSKEW_SIZE, 255) != -1)
                   disk.pskew0_opt = 1;
                else
                   param_error_mesg();
                break;
             case OPT_PSKEW1 :
                if (get_int_arguments(e_optarg, disk.pskew1,
                PSKEW_SIZE, 255) != -1)
                   disk.pskew1_opt = 1;
                else
                   param_error_mesg();
                break;                
             case OPT_RETRY_L1 :
                set_int_from_arg(&disk.retries_l1, 1, 1000);
                break;
             case OPT_RETRY_L2 :
                set_int_from_arg(&disk.retries_l2, 1, 1000);
                break;
             case OPT_RWGAP :
                set_int_from_arg(&dg_opts.rwgap, 0, 1000000);
                break;
             case OPT_SCANHEAD :
                if (scan_option(e_optarg) == -1)
                   param_error_mesg();
                break;
             case OPT_SECBASE :
                set_int_from_arg(&dg_opts.secbase, 0, 1000000);
                break;
             case OPT_SECBASE2S :
                set_int_from_arg(&dg_opts.secbase2s, 0, 1000000);
                break;
             case OPT_SECBASE2C :
                set_int_from_arg(&dg_opts.secbase2c, 0, 1000000);
                break;
             case OPT_SECSIZE :
                if ((int_arg != 0) && !(int_arg & (int_arg - 1)) &&
                (int_arg >= 128))
                   dg_opts.secsize = int_arg;
                else   
                   param_error_mesg();
                break;                                      
             case OPT_SECTORS :
                set_int_from_arg(&dg_opts.sectors, 0, 1000000);
                break;
             case OPT_SIDE1AS0 :
                set_int_from_arg(&dg_opts.side1as0, 0, 1);
                break;
             case OPT_SIDEDNESS :
#if 0
                set_int_from_list(&dg_opts.sidedness, sidedness_args);
#else
                printf(APPNAME": --sidedness option has been deprecated.\n");
#endif
                break;
             case OPT_SIDEOFFS :
                set_int_from_arg(&dg_opts.sideoffs, 0, 255);
                break;
             case OPT_SIGNATURE :
                strcpy(disk.signature, e_optarg);
                break;
             case OPT_SKEW :
                set_int_from_arg(&dg_opts.skew_val, 1, 1000000);
                break;
             case OPT_SKEW_OFS :
                set_int_from_arg(&dg_opts.skew_ofs, 0, 1000000);
                break;
             case OPT_START :
                set_int_from_arg(&disk.start, 0, 1000000);
                break;
             case OPT_SFMODE :
                set_int_from_list(&disk.sfmode, sfmode_args);
                break;
             case OPT_VERSION :
                printf(APPVER"\n");
                exitstatus = 1;
                break;

             case OPT_UNATTENDED :
                set_int_from_list(&disk.unattended, offon_args);
                break;
             case OPT_UNATTRAB :
                set_int_from_arg(&disk.unattended_retry_abort_max, 0, 1000000);
                break;
             case OPT_UNATTRPS :
                set_int_from_arg(&disk.unattended_retry_sector_max, 0, 1000000);
                break;
             case OPT_UNATTSEEK :
                set_int_from_arg(&disk.unattended_seeked_max, 0, 1000000);
                break;
             case OPT_VARSET :
                options_ubeedisk_envvar_set(e_optarg);
                break;
             case OPT_VARUSET :
                options_ubeedisk_envvar_unset(e_optarg);
                break;
             case OPT_VERBOSE :
                set_int_from_arg(&disk.verbose, 0, 1000000);
                break;
            }
        }
    }

 // Check if any other arguments are incorrectly specified
 if ((optind < argc) && (exitstatus == 0))
    {
     exitstatus = 1;
     printf("%s: %d additional arguments were specified that are "
            "not recognised:\n", argv[0], argc - optind);
     printf("%s: ", argv[0]);
     while (optind < argc)
        printf("%s ", argv[optind++]);
     printf("\n");
     printf(TRY_MESG, argv[0], argv[0], argv[0]);
    }
}

//==============================================================================
// Allocate memory.
//
//   pass: int size
// return: char *                       result of memory allocate
//==============================================================================
static char *options_malloc (int size)
{
 char *ptr;
 
 if (! (ptr = malloc(size)))
    {
     exitstatus = 1;
     printf("options_malloc: unable to malloc %d bytes of memory\n", size);
    } 

 return ptr; 
}

//==============================================================================
// Build a list of definitions found in the configuration file.
//
//   pass: void
// return: void
//==============================================================================
static void options_buildlist (void)
{
 int l;
 char s[OPTIONS_SIZE];
 
 if (! fp)
    return;

 fseek(fp, 0, SEEK_SET);

 while ((l = file_readline(fp, s, OPTIONS_SIZE)))
    {
     if ((s[0] == '[') && (s[l-1] == ']'))
        {
         if (! (ndefsv[ndefsc] = options_malloc(l-1)))  // space for name + 0
            return;
         strncpy(ndefsv[ndefsc], &s[1], l-2);
         ndefsv[ndefsc++][l-2] = 0;
        }
    }
}

//==============================================================================
// Find a definitions entry in the names list.
//
//   pass: const char *name             name of the definition to look for
// return: int                          0 if no match found, else index +1
//==============================================================================
static int options_findentry (const char *name)
{
 int i = 0;
 
 if (! fp)
    return 0;

 while ((i < ndefsc) && (strcmp(ndefsv[i], name) != 0))
    i++;
    
 if (i < ndefsc)
    return i+1;
    
 return 0;
}

//==============================================================================
// Get options for the named entry from the configuration file.
//
//   pass: const char *name             definition name
//         char *options                return options found for definition
//                                      string if entry not found or empty
// return: void
//==============================================================================
static void options_getoptstr (const char *name, char *options)
{
 int l;
 int finish = 0;

 char s[OPTIONS_SIZE];
       
#ifdef DARWIN
    char s1[OPTIONS_SIZE];
#endif

 if (! fp)
    return;

 fseek(fp, 0, SEEK_SET);

 options[0] = 0;

 // find the definition entry 
 while ((! finish) && (l = file_readline(fp, s, OPTIONS_SIZE)))
    {
     if ((s[0] == '[') && (s[l-1] == ']'))
        {
            #ifdef DARWIN
                strncpy(s1, &s[1], l-2);
                s1[l-2] = 0;
                finish = (strcmp(s1, name) == 0);
            #else
                strncpy(s, &s[1], l-2);
                s[l-2] = 0;
                finish = (strcmp(s, name) == 0);
            #endif
        }
    }

 if (! finish)
    return;

 finish = 0;

 // concatenate all the definition entries into one string
 while (! finish)
    {
     if ((l = file_readline(fp, s, OPTIONS_SIZE)))
        {
         if (! (finish = ((s[0] == '[') && (s[l-1] == ']'))))
            {
             l = OPTIONS_SIZE - (strlen(options) + 1);
             if (l > 0)
                {
                 strcat(options, " ");
                 strncat(options, s, l-1);
                } 
            }
        }
     else
        finish = 1;
    }

 return;   
}

//==============================================================================
// Parse the options from the config file entry string and place into a
// xargv variable.
//
//   pass: const char *options          options to be parsed
// return: void
//==============================================================================
static void options_parse (const char *options)
{
 int i = 0;
 int l;

 char s[OPTIONS_SIZE];
 
 while (options[i])
    {
     while ((options[i]) && (options[i] <= ' '))  // move past white space
        i++;

     l = 0;   
     while ((options[i]) && (options[i] != ' ') && (options[i] != '"'))     
        {
#ifdef WIN32
#else
         if (options[i] == '\\')
            i++;
#endif   
         if (options[i])
            {
             s[l++] = options[i];
             i++;
            } 
        }
     if (options[i] == '"')
        {
         i++;
         while ((options[i]) && (options[i] != '"'))
            {
             s[l++] = options[i];
             i++;
            }
         i++;   
        }   
     s[l] = 0;
     
     if (! (xargv[xargc] = options_malloc(l+1)))  // space for argument + 0
        return;
     strcpy(xargv[xargc++], s);
    }
}

//==============================================================================
// Process early options found on the command line, these are special options
// and if used must be declared before all others.  These options will be
// ignored by the options_getopt() function.
//
// --config option:
// By default the ubeedisk.ini file found in the home account is used unless
// another file is specified. If this option is used it must be the first or
// second option on the command line.
//
// Note: The number of arguments (argc) consists of the number of command
// line space delimited arguments (except where quoted), the program name is
// also included in the count.  The first argument is therefore the second
// argv pointer.
//
//   pass: char *s                      pointer to string
//         int argc                     number of arguments
//         char *argv[]                 pointer to pointer to arguments
// return: int                          0 if no error, 1 if error, neg if exit
//==============================================================================
static int options_early (char *s, int argc, char *argv[])
{
 int argv_pos = 1;

 // test if --config is the next command line option,  this is used
 // to set an alternative configuration file.
 if (argc > 1)
    {
     // option & arg may be 1 entry (i.e. --option=arg)
     if (strncmp("--config=", argv[argv_pos], 9) == 0)
        strcpy(s, &argv[argv_pos][9]);
     else
        // option & arg may be broken up over 2 entries (i.e. --option arg)
        if (strcmp("--config", argv[argv_pos]) == 0)
           strcpy(s, argv[++argv_pos]);
    }

 return 0;   
}

//==============================================================================
// Process all options from the pointer list and the configuration file.
//
//   pass: int argc                     number of arguments
//         char *argv[]                 pointer to pointer to arguments
// return: int                          0 if no error, 1 if error, neg if exit
//==============================================================================
int options_process (int argc, char *argv[])
{
 int i;

 char filepath[SSIZE1];
 char file_options[OPTIONS_SIZE];
 char file_section[40];

 exitstatus = 0;

 // close any open configuration file
 if (fp)
    fclose(fp);

 // free existing pointers in use for section entry names list
 while (ndefsc)
    free(ndefsv[--ndefsc]);

 // free existing pointers in use for arguments
 while (xargc)
    free(xargv[--xargc]);

 // set conditionals state
 if_pos = 0;
 if_state[if_pos] = 1;  // true at the base level

 // set default configuration file to use if none was set previously
 if (! config_file[0])
    strcpy(config_file, APPNAME".ini");

 // process any special early options from the command line, the
 // configuration file may be changed with an option here.
 i = options_early(config_file, argc, argv);
 if (i)
    return i;

 // if use no configuration file has been requested then NULL the file pointer
 if (strcmp("none", config_file) == 0)
    fp = NULL;
 else
    {
     fp = open_file(config_file, userhome_confpath, filepath, "r");
     if (strlen(config_file) && (! fp))
        {
         fp = NULL;
         printf(APPNAME": Warning - The configuration file '%s' was not found\n",
         filepath);
        }
    }

 // build a list of the definitions contained in the configuration file
 options_buildlist();
 if (exitstatus)
    return exitstatus;

 // first argument will be the program name and must be inserted first
                               // space for argument + 0
 if (! (xargv[xargc] = options_malloc(strlen(argv[0])+1)))
    return exitstatus;

 strcpy(xargv[xargc++], argv[0]);

 // process the configuration file's 'global-start' options
 strcpy(file_section, "global-start");
 if (options_findentry(file_section))
    {
     options_getoptstr(file_section, file_options);
     if (strlen(file_options))
        options_parse(file_options);
    }
 if (exitstatus)
    return exitstatus;
    
 // process all the command line and configuration file arguments after the
 // program name
 for (i = 1; i < argc; i++)
    {
     // if it's a command line option (-) or the entry is not a section entry
     if ((*argv[i] == '-') || (! options_findentry(argv[i])))
        {                               // space for argument + 0
         if (! (xargv[xargc] = options_malloc(strlen(argv[i])+1))) 
            return exitstatus;

         strcpy(xargv[xargc++], argv[i]);
        }
     else
        {
         // otherwise if a section entry grab all the options from the file
         if (options_findentry(argv[i]))
            {
             options_getoptstr(argv[i], file_options);
             if (strlen(file_options))
                options_parse(file_options);
            }
        }
    }

 // process the configuration file's 'global-end' options
 strcpy(file_section, "global-end");
 if (options_findentry(file_section))
    {
     options_getoptstr(file_section, file_options);
     if (strlen(file_options))
        options_parse(file_options);
    }

 // process all the options
 if (! exitstatus)
    options_getopt(xargc, xargv);

 return exitstatus;
}
