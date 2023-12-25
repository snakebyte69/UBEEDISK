//******************************************************************************
//*                                 uBeeDisk                                   *
//*                                                                            *
//*     A tool for converting disks/images from one to another with auto       *
//*                   detection options for Microbee disks.                    *
//*                                                                            *
//*                            functions module                                *
//*                                                                            *
//*                       Copyright (C) 2008-2017 uBee                         *
//******************************************************************************
//
// Various functions that don't belong anywhere else.
//
//==============================================================================
// ChangeLog (most recent entries are at top)
//==============================================================================
// v4.0.0 - 25 January 2017, uBee
// - Added endian support functions.
// - Added get_colon_arguments(), get_gap_colon_arguments() and
//   get_int_arguments() functions.
// - Added a memstr() function.
// - Added win32 strcasestr() and strverscmp() functions.
//
// v3.0.0 - 4 January 2015, uBee
// - Added string_case_search() and string_struct_search*() functions.
//
// v2.2.0 - 14 May 2012, uBee
// - Added xdrive parameter to get_drive_type() and code to enquire from
//   remote devices what the drive type is.
// - Added function_init() and function_deinit(). These currently
//   pre-configure and reinstate Unix stdin values.
// - Change Unix get_key() to use the more modern termios methods and
//   different configurations to prevent unwanted key echo.
// - Added a fgetsx() wrapper for fgets() and printfx() wrapper for printf()
//   which temporarily enables stdout echo under Unices.
//
// v2.0.0 - 25 September 2011, uBee
// - Added get_key() function and support code for Unix POSIX and windows
//   systems to test for a key using non-blocking methods.
// - Added time_get_ms() and sleep_ms() functions.
// - Added get_integer_value() and get_next_parameter() functions.
//
// v1.4.0 - 24 February 2010, uBee
// - Added toupper_string() and tolower_string() functions.
//
// v1.0.0 - 22 December 2008, uBee
// - Created new file.

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
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <stdint.h>

#ifdef WIN32
#include <windows.h>
#include <winioctl.h>
#include <conio.h>
#include <time.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "termio.h"     // used to set terminal modes
#include <termios.h>
#include <sys/types.h>  // various type definitions, like pid_t
#include <sys/time.h>
#endif

#ifdef __linux__
#include <linux/fd.h>
#endif

#ifdef __FreeBSD__
#include <sys/fdcio.h>
#endif

#include "ubeedisk.h"
#include "functions.h"
#include "md5.h"

//==============================================================================
// structures and variables
//==============================================================================
#ifdef WIN32
#else
 static struct timeval tod;
 struct termios term, tOrg;
#endif

//==============================================================================
// Test if system uses Big Endian or Little Endian format.
//
//   pass: void
// return: int                  1 if Big Endian, 0 if Little Endian
//==============================================================================
int IsBigEndian (void)
{
#if defined(__aix__)
 return 1;
#else
 int i = 1;
 int c = *((char*)&i);
 return c == 0;
#endif
}

//==============================================================================
// Swap Endianess. Only works for even number variable sizes.
//
//   pass: unsigned char *na
//         int size             size in bytes of the variable
// return: void
//==============================================================================
void swap_endian (unsigned char *na, int size)
{
 int i;
 int x;
 unsigned char *nb = na + (size - 1);  // pointer to end of variable

 for (i = 0; i < (size / 2); i++)
    {
     x = *na;
     *na++ = *nb;
     *nb-- = x;
    }
}

//==============================================================================
// Little Endian to host endian,  and host endian to Little Endian functions.
// Swaps byte order if on a Big Endian host.
//
// Don't use these functions directly.  use the macros defined in the header
// file as these make the usage clearer.
//
//   pass: xintxx_t n                   little endian, or host endian format
// return: xintxx_t                     little endian format
//==============================================================================
int16_t little_endian_16 (int16_t n)
{
 if (IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

uint16_t little_endian_u16 (uint16_t n)
{
 if (IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

int32_t little_endian_32 (int32_t n)
{
 if (IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

uint32_t little_endian_u32 (uint32_t n)
{
 if (IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

int64_t little_endian_64 (int64_t n)
{
 if (IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

uint64_t little_endian_u64 (uint64_t n)
{
 if (IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

//==============================================================================
// Big Endian to host endian,  and host to Big Endian functions.
// Swaps byte order if on a Little Endian host.
//
//   pass: xintxx_t n                   big endian, or host endian format
// return: xintxx_t                     host endian, or big endian format
//==============================================================================
int16_t big_endian_16 (int16_t n)
{
 if (! IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

uint16_t big_endian_u16 (uint16_t n)
{
 if (! IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

int32_t big_endian_32 (int32_t n)
{
 if (! IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

uint32_t big_endian_u32 (uint32_t n)
{
 if (! IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

int64_t big_endian_64 (int64_t n)
{
 if (! IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

uint64_t big_endian_u64 (uint64_t n)
{
 if (! IsBigEndian())
    swap_endian((unsigned char *)&n, sizeof(n));
 return(n);
}

#ifdef WIN32
//==============================================================================
// get_key.
//
// Test if a key is ready and return it if there is.
//
//   pass: void
// return: key if ready else -1.
//==============================================================================
int get_key (void)
{
 if (kbhit())
    return getch();
 return -1;
}
#else
//==============================================================================
// get_key() function for standard Unix like systems.  This works in a
// similar way as for the WIN32 method.
//
// See function_init() for the pre-configured variables.
//
// Some keys need to be filtered and other values returned, this may be able
// to be changed by correct configuration.
//
//   pass: void
// return: int key if ready else -1.
//==============================================================================
int get_key (void)
{
 int ch;

 // no delays on reading stdin
 tcsetattr(fileno(stdin), TCSANOW, &term);

 ch = getchar();

 while (getchar() != -1)
    ;
    
 switch (ch)
    {
     case 0x7f : // Unix returns ch == 127 on BS key
        return 8;
     case 0x0a : // Unix returns ch == 10 on ENTER key
        return 13;
     default :
        return ch;
    }
}

//==============================================================================
// printf() wrapper.
//
// Temporarily switches back to default stdin values under unices so that
// output echoes \r to stdout.
//
//   pass: char * fmt,...
// return: int                  value from vsnprintf() function
//==============================================================================
int printfx (char * fmt, ...)
{
 int res;
 va_list ap;
 char buffer[100000];

 va_start(ap, fmt);
 vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
 buffer[sizeof(buffer)-1] = 0;
 va_end(ap);

 tcsetattr(fileno(stdin), TCSANOW, &tOrg);
 res = fprintf(stdout, "%s", buffer);
 tcsetattr(fileno(stdin), TCSANOW, &term);

 return res;
}

//==============================================================================
// fgets() wrapper.
//
// Temporarily switches back to default stdin values under unices so that
// input is echoed to stdout.
//
//   pass: char *s
//         int size
//         FILE *stream
// return: void
//==============================================================================
char *fgetsx (char *s, int size, FILE *stream)
{
 if (stream == stdin)
    tcsetattr(fileno(stdin), TCSANOW, &tOrg);

#undef fgets
 return fgets(s, size, stream);
#define fgets fgetsx

 if (stream == stdin)
    tcsetattr(fileno(stdin), TCSANOW, &term);
}

#endif

//==============================================================================
// Sleep for milliseconds.
//
//   pass: int ms                       number of milliseconds
// return: void
//==============================================================================
void sleep_ms (int ms)
{
#ifdef WIN32
 Sleep(ms);
#else
 usleep(ms * 1000);
#endif            
}

//==============================================================================
// Get the current clock time in milliseconds
//
//   pass: void
// return: uint64_t                     number of milliseconds
//==============================================================================
uint64_t time_get_ms (void)
{
#ifdef WIN32
 return (uint64_t)clock();
#else
 gettimeofday(&tod, NULL);
 return (((uint64_t)tod.tv_sec * 1000) + ((uint64_t)tod.tv_usec / 1000));
#endif
}

#ifdef WIN32
//==============================================================================
// strcasestr for systems that don't have this function.
//
//   pass: char *haystack
//         char *needle
// return: char *
//
// Returns a pointer to the beginning of the substring, or NULL if the
// substring is not found. 
//==============================================================================
char *strcasestr (char *haystack, char *needle)
{
 char *p, *startn = 0, *np = 0;

 for (p = haystack; *p; p++)
    {
     if (np)
        {
         if (toupper(*p) == toupper(*np))
            {
             if (!*++np)
                return startn;
            }
         else
            np = 0;
        }
     else
        if (toupper(*p) == toupper(*needle))
           {
            np = needle + 1;
            startn = p;
           }
    }
 return NULL;
}

//==============================================================================
// Compare strings while treating digits characters numerically.
//
// Compare S1 and S2 as strings holding indices/version numbers, returning
// less than, equal to or greater than zero if S1 is less than, equal to or
// greater than S2 (for more info, see the texinfo doc).
//
// This code has been included as mingw does not appear to have a function
// to do this.
//
// Copyright (C) 1997, 2000, 2002, 2004, 2006 Free Software Foundation, Inc.
// This function is part of the GNU C Library.  Contributed by Jean-François
// Bignolles <bignolle@ecoledoc.ibp.fr>, 1997.
//
//   pass: const char *s1
//         const char *s2
// return: int
//==============================================================================
// states: S_N: normal, S_I: comparing integral part, S_F: comparing
//           fractional parts, S_Z: idem but with leading Zeroes only

#define S_N    0x0
#define S_I    0x4
#define S_F    0x8
#define S_Z    0xC

// result_type: CMP: return diff; LEN: compare using len_diff/diff
#define CMP    2
#define LEN    3

// ISDIGIT differs from isdigit, as follows:
//  - Its arg may be any int or unsigned int; it need not be an unsigned char
//    or EOF.
//  - It's typically faster.
//  POSIX says that only '0' through '9' are digits.  Prefer ISDIGIT to
//  isdigit unless it's important to use the locale's definition
//  of `digit' even when the host does not conform to POSIX.

#define ISDIGIT(c) ((unsigned int) (c) - '0' <= 9)

int strverscmp (const char *s1, const char *s2)
{
 const unsigned char *p1 = (const unsigned char *) s1;
 const unsigned char *p2 = (const unsigned char *) s2;
 unsigned char c1, c2;
 int state;
 int diff;

 // Symbol(s)    0       [1-9]   others  (padding)
 // Transition   (10) 0  (01) d  (00) x  (11) -
 static const unsigned int next_state[] =
 {
     /* state    x    d    0    - */
     /* S_N */  S_N, S_I, S_Z, S_N,
     /* S_I */  S_N, S_I, S_I, S_I,
     /* S_F */  S_N, S_F, S_F, S_F,
     /* S_Z */  S_N, S_F, S_Z, S_Z
 };

 static const int result_type[] =
 {
     /* state   x/x  x/d  x/0  x/-  d/x  d/d  d/0  d/-
                0/x  0/d  0/0  0/-  -/x  -/d  -/0  -/- */

     /* S_N */  CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
                CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
     /* S_I */  CMP, -1,  -1,  CMP,  1,  LEN, LEN, CMP,
                 1,  LEN, LEN, CMP, CMP, CMP, CMP, CMP,
     /* S_F */  CMP, CMP, CMP, CMP, CMP, LEN, CMP, CMP,
                CMP, CMP, CMP, CMP, CMP, CMP, CMP, CMP,
     /* S_Z */  CMP,  1,   1,  CMP, -1,  CMP, CMP, CMP,
                -1,  CMP, CMP, CMP
 };

 if (p1 == p2)
    return 0;

 c1 = *p1++;
 c2 = *p2++;

 // Hint: '0' is a digit too.
 state = S_N | ((c1 == '0') + (ISDIGIT (c1) != 0));

 while ((diff = c1 - c2) == 0 && c1 != '\0')
    {
     state = next_state[state];
     c1 = *p1++;
     c2 = *p2++;
     state |= (c1 == '0') + (ISDIGIT (c1) != 0);
    }

 state = result_type[state << 2 | ((c2 == '0') + (ISDIGIT (c2) != 0))];

 switch (state)
    {
     case CMP:
        return diff;

        case LEN:
           while (ISDIGIT (*p1++))
              if (!ISDIGIT (*p2++))
                 return 1;

           return ISDIGIT (*p2) ? -1 : diff;

        default:
           return state;
    }
}
#endif

//==============================================================================
// Copy a string to upper case.  Source and destination may be the same.
//
//   pass: char *dest                   destination string
//         char *src                    source string
// return: void
//==============================================================================
void toupper_string (char *dest, char *src)
{
 int i = 0;
 
 while (src[i])
    {
     dest[i] = toupper(src[i]);
     i++;
    }
 dest[i] = 0;
}

//==============================================================================
// Copy a string to lower case.  Source and destination may be the same.
//
//   pass: char *dest                   destination string
//         char *src                    source string
// return: void
//==============================================================================
void tolower_string (char *dest, char *src)
{
 int i = 0;
 
 while (src[i])
    {
     dest[i] = tolower(src[i]);
     i++;
    }
 dest[i] = 0;
}

//==============================================================================
// Search an array of strings for the first occurence of the passed search
// string.  The string array must be be terminated by an empty string.
//
//   pass: char *strg_array     pointer to a string array to be checked
//         char *strg_find      pointer to a string to search for
// return: int                  index if found, else -1
//==============================================================================
int string_search (char *strg_array[], char *strg_find)
{
 int i;

 for (i = 0; strg_array[i][0] != 0; i++)
    {
     if (strcmp(strg_find, strg_array[i]) == 0)
        return i;
    }
 return -1;
}

//==============================================================================
// Same as string_search() but matches upper and lower case.
//
//   pass: char *strg_array     pointer to a string array to be checked
//         char *strg_find      pointer to a string to search for
// return: int                  index if found, else -1
//==============================================================================
int string_case_search (char *strg_array[], char *strg_find)
{
 int i;

 for (i = 0; strg_array[i][0] != 0; i++)
    {
     if (strcasecmp(strg_find, strg_array[i]) == 0)
        return i;
    }
 return -1;
}

//==============================================================================
// Search an array of strings with associated values in any case for the
// first occurence of the passed search string.  The string array must be
// terminated by an empty string.
//
//   pass: sup_args_t *args     pointer to array structure
//         char *strg_find      pointer to a string to search for
// return: int                  index if found, else -1
//==============================================================================
int string_struct_search (sup_args_t *args, char *strg_find)
{
 int x = 0;

 while (*args[x].name && strcasecmp(args[x].name, strg_find) != 0)
    x++;

 if (! *args[x].name)
    return -1;

 return x;
}

//==============================================================================
// Search an array of integers with associated strings for the first
// occurence of the passed integer value.  The string array must be
// terminated by an empty string.
//
//   pass: sup_args_t *args     pointer to array structure
//         int value            integer 'value' to search for
// return: int                  index if found, else -1
//==============================================================================
int string_struct_search_i (sup_args_t *args, int value)
{
 int x = 0;

 while (*args[x].name && args[x].value != value)
    x++;

 if (! *args[x].name)
    return -1;

 return x;
}

//==============================================================================
// Search a binary array for the first occurence of pattern. This is
// funtionally the same as the standard strstr() function except NULL
// characters are also searched.
//
//   pass: void *m              pointer to block of memory to be searched
//         int n                size of the block
//         char *s              pattern to be searched for
// return: char *               pointer to found position or NULL if not found
//==============================================================================
char *memstr (void *m, int n, char *s)
{
 char *found;
 void *p;
 int done = 0;

 p = m;
 while (! done)
    {
     found = memchr(p, *s, n - (int)(p - m));
     if (found == NULL)
        done++;
     else
        if (memcmp(found, s, strlen(s)) == 0)
           done++;
     p = found + 1;
    }
 return found;
}

//==============================================================================
// Convert string to an integer value with error checking.  A -1 value is
// returned if an error is found.
//
// The function uses the strtol() function with the BASE value set to 0 to
// use auto base detection.
//
//   pass: char *s                      string to be converted
// return: int                          integer value
//==============================================================================
int get_integer_value (char *s)
{
 char *check;
 int i;

 if (*s == 0)  // if an empty string
    return -1;

 i = (int)strtol(s, &check, 0);

 if (*check != 0)  // if not end of string then it's an error
    i = -1;

 return i;
}

//==============================================================================
// Parse a string and return the next sub-parameter as a string and an
// integer value.  A pointer to the next parameter position or a NULL is
// returned if the end of string is reached.
//
//   pass: char *s                      string to be parsed
//         int delimiter                sub parameter delimiter
//         char *sps                    sub parameter string returned here
//         int spi                      sub parameter integer returned here
//         int maxlen                   maximum length allowed
// return: char *s                      pointer to next entry
//==============================================================================
char *get_next_parameter (char *s, int delimiter, char *sps, int *spi,
                          int maxlen)
{
 int c = 0;
 char *sps_x = sps;

 if (s == NULL)
    {
     *spi = -1;
     *sps = 0;
     return NULL;
    }

 while ((*s) && (*s != delimiter) && (c++ < maxlen))
    *(sps++) = *(s++);
 *sps = 0;

 *spi = get_integer_value(sps_x);

 if (*s)
    {
     s++;      // get past the delimiter
     if (! *s) // check if delimiter was at end of the line
        return NULL;
     return s;
    }
 else
    return NULL;
}

//==============================================================================
// Process --option=n,n,n... command line parameters.  Places 'n' values
// into an integer array pointed to by the pointer passed.  A maximum number
// of entries and value number is also passed.
//
//   pass: char *parms                  command parameters
//         int *p                       pointer to a values array
//         int max_ent                  maximum number of array entries
//         int max_val                  maximum data number value
// return: int                          -1 if error else 0
//==============================================================================
int get_int_arguments (char *parms, int *p, int max_ent, int max_val)
{
 char s[10];
 char *c = NULL;
 char *c2 = parms;
 int v;

 for (;;)
    {
     if (! max_ent)
        return -1;
        
     c = get_next_parameter(c2, ',', s, &v, sizeof(s)-1);    
     if (v < 0 || v > max_val)
        return -1;

     *(p++) = v;
     c2 = c;
     
     //printf("Value=%d\n", *(p-1));
     
     // exit if no more values
     if (! c)
        return 0;
        
     max_ent--;
    }
}

//==============================================================================
// Process --option=e:v,[e:v...] command line parameters.  An options string
// is parsed and the values inserted into an array pointed to by the pointer
// passed.  A maximum number of entries and value number is also passed.
//
// 'e' is the entry number (1 is the first entry), 'v' is the value to be
// placed into the array.  The array pointer must be of type integer.
//
//   pass: char *parms                  command parameters
//         int *p                       pointer to a values array
//         int max_ent                  maximum number of array entries
//         int max_val                  maximum data number value
// return: int                          -1 if error else 0
//==============================================================================
int get_colon_arguments (char *parms, int *p, int max_ent, int max_val)
{
 char s[10];
 char s2[10];
 char *c = NULL;
 char *c2 = NULL;
 int e;
 int v;
 int x;
 
 for (;;)
    {
     if (! c)
        c = get_next_parameter(parms, ',', s, &x, sizeof(s)-1);
     else
        c = get_next_parameter(c, ',', s, &x, sizeof(s)-1);
        
     // there must be a ':' separating the a:b values
     if (! strcasestr(s, ":"))
        return -1;

     // get the entry value 
     c2 = get_next_parameter(s, ':', s2, &e, sizeof(s2)-1);
     if (e < 1 || e > max_ent)
        return -1;

     c2 = get_next_parameter(c2, ',', s2, &v, sizeof(s2)-1);
     if (v < 0 || v > max_val)
        return -1;
        
     p[e-1] = v;
     
     //printf("Entry=%d Value=%d\n", e, p[e-1]);
     
     // exit if no more values
     if (! c)
        return 0;
    }
}

//==============================================================================
// Process --option=pe:v,[pe:v...] command line parameters.  An options
// string is parsed and the values inserted into an integer array pointed to
// by the pointer passed.  A maximum number of entries and value number is
// also passed.
//
// This function is similar to get_colon_arguments() but is intended only
// for GAP/SYNC values using a prefix 'p' character of 'g' or 's'.
//
// 'e' is the entry number (1 is the first entry), 'v' is the value to be
// placed into the array.  The array pointer must be of type integer and
// each entry number requires 2 integers.  The first entry is the first GAP
// position followed by the SYNC value.
//
// e:  1   2   3   4
// x:  g s g s g s g s
//
// The array index is: i = e * 2 + sync - 1
//
//   pass: char *parms                  command parameters
//         int *p                       pointer to GAP/SYNC values array
//         int max_ent                  maximum number of array entries
//         int max_val                  maximum data number value
// return: int                          -1 if error else 0
//==============================================================================
int get_gap_colon_arguments (char *parms, int *p, int max_ent, int max_val)
{
 char s[10];
 char s2[10];
 char *c = NULL;
 char *c2 = NULL;
 char *w;

 int sync;
 int e;
 int v;
 int i;
 int x;
 
 for (;;)
    {
     if (! c)
        c = get_next_parameter(parms, ',', s, &x, sizeof(s)-1);
     else
        c = get_next_parameter(c, ',', s, &x, sizeof(s)-1);
        
     // there must be a ':' separating the a:b values
     if (! strcasestr(s, ":"))
        return -1;

     // get the entry value with leading prefix
     c2 = get_next_parameter(s, ':', s2, &e, sizeof(s2)-1);
     tolower_string(s2, s2);

     w = s2;
     
     // get past any white space
     while (*w && *w <= ' ')
        w++;

     // must have a leading 'g' or 's' prefix
     if (*w != 'g' && *w != 's')
        return -1;
     
     sync = *w == 's';

     // now convert the integer part of the string
     e = get_integer_value(w + 1);
        
     if (e < 1 || e > max_ent)
        return -1;
        
     c2 = get_next_parameter(c2, ',', s2, &v, sizeof(s2)-1);
     if (v < 0 || v > max_val)
        return -1;

     i = (e - 1) * 2 + sync;
     p[i] = v;
     
     //printf("%s%d Value=%d Index=%d\n", sync? "SYNC":" GAP", e, p[i], i);
     
     // exit if no more values
     if (! c)
        return 0;
    }
}

//==============================================================================
// Return the file name part from the end of a file path.
//
//   pass: char *pathfile               path and file name
//         char *filename               file name part
// return: void
//==============================================================================
void file_name_part (char *pathfile, char *filename)
{
 int i;
 
 // we only want file names, not the complete paths
 i = strlen(pathfile);
 while (i)
    {
     i--;
#ifdef WIN32
     if (pathfile[i] == '\\')
#else
     if (pathfile[i] == '/')
#endif     
        {
         i++; 
         break;
        } 
    }
 strcpy(filename, pathfile+i);
}

//==============================================================================
// Create an MD5 message digest.
//
//   pass: char *filename               file to create MD5 for
//         char *md5                    pointer to a 33 byte buffer for MD5
// return: void
//==============================================================================
void create_md5 (char *filename, char *md5)
{
 FILE *fp;
 uint8_t resblock[16];
 int i;

 md5[0] = 0;
 fp = fopen(filename, "rb");
 if (fp)
    {
     if (md5_stream(fp, &resblock) == 0)
        {
         for (i = 0; i < 16; i++)
            sprintf(md5, "%s%02x", md5, resblock[i]);
        } 
     fclose(fp);        
    } 
}

//==============================================================================
// Check operating system version
//
//   pass: void
// return: int                                  0 if ok, else -1
//==============================================================================
int check_os_version (void)
{
#ifdef WIN32
 // get host system version information
 OSVERSIONINFO osvi;

 ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
 osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
 GetVersionEx(&osvi);

 // make sure we are on running W2000 or higher
 if (osvi.dwMajorVersion >= 5)
    return 0;

 printf("This program must be run on Windows 2000, XP, Vista or higher\n");
 printf("and requires the 'ntwdm' floppy driver to be installed.\n");
 return -1;
 
#else
 return 0;
#endif
}

#ifdef WIN32
//==============================================================================
// Get Windows floppy type
//
//   pass: HANDLE hDrive
// return: int                          floppy type (as per Linux)
//==============================================================================
static int get_floppy_type (HANDLE *hDrive)
{
 DISK_GEOMETRY SupportedGeometry[20];

 DWORD ReturnedByteCount;
 int drive_type = 0;
 int geometrys;
 int i;

 if (DeviceIoControl(
    hDrive,
    IOCTL_DISK_GET_MEDIA_TYPES,
    NULL,
    0,
    SupportedGeometry,
    sizeof(SupportedGeometry),
    &ReturnedByteCount,
    NULL
    ))
       geometrys = ReturnedByteCount /  sizeof(DISK_GEOMETRY);
    else
       return 0;

 for (i = 0; i < geometrys; i++)
    {
     switch (SupportedGeometry[i].MediaType)
        {
         // 3.5" drive types
         case F3_1Pt44_512: // 3.5, 1.44MB, 512 bytes/sector
            if (drive_type != 'E')
               drive_type = 'H';
            break;
         case F3_2Pt88_512: // 3.5, 2.88MB, 512 bytes/sector
            drive_type = 'E';
            break;
         case F3_20Pt8_512: // 3.5, 20.8MB, 512 bytes/sector
            break;
         case F3_720_512:   // 3.5, 720KB, 512 bytes/sector
            if ((drive_type != 'H') && (drive_type != 'E'))
               drive_type = 'D';
            break;

         // 5.25" drive types
         case F5_1Pt2_512:  // 5.25, 1.2MB, 512 bytes/sector
            drive_type = 'h';
            break;
         case F5_360_512:   // 5.25, 360KB, 512 bytes/sector
         case F5_320_512:   // 5.25, 320KB, 512 bytes/sector
         case F5_320_1024:  // 5.25, 320KB, 1024 bytes/sector
         case F5_180_512:   // 5.25, 180KB, 512 bytes/sector
         case F5_160_512:   // 5.25, 160KB, 512  bytes/sector
            if (drive_type != 'h')
               drive_type = 'd';
            break;

         case RemovableMedia: // Removable media other than floppy
            break;
         case FixedMedia: // Fixed hard disk media
            break;
         default: // Unknown
            break;
        }
    }

  return drive_type;
}
#endif

//==============================================================================
// Reset drive.
//
// Found that this is necessary in Linux when LibDsk is interrupted with a
// ^C and when re-running the application the track position is incorrect,
// this can be seen with a disk scan.  The problem can be corrected by
// ejecting and replacing the disk or by calling ioctl(fd, FDCLRPRM).
//
// This may not be needed in later versions of LibDsk as some similar code
// has been added since LibDsk-1.2.1
//
//   pass: char *xtype                          input or output type
//         char *xfile                          input or output file
// return: void
//==============================================================================
void reset_drive (char *xtype, char *xfile)
{
#ifdef __linux__
 int fd;
 
 if (strcmp(xtype, "floppy") != 0)
    return;

 fd = open(xfile, O_RDONLY | O_NDELAY);

 if (fd)
    {
     ioctl(fd, FDCLRPRM);
     close(fd);
    }
#endif 
}

//==============================================================================
// Get floppy drive information if xtype is floppy, ntwdm or remote driver.
//
// Returns a letter indicating the drive type.  These values are based on the
// Linux FD FDGETDRVTYP ioctl call.  Windows and FreeBSD will be made to
// return the same corresponding values.
//
// Remote devices (i.e. Floppyio) that support the DRIVETYPE option may be
// used to return the drive type letter.
//
// Linux:
// dnnnn = 5.25 inch double density device files
// hnnnn = 5.25 inch high density device files
// Dnnnn = 3.5 inch double density device files
// Hnnnn = 3.5 inch high density device files
// Ennnn = 3.5 inch extra density device files
//
//   pass: char *xtype                          input or output type
//         char *xfile                          input or output file
//         DSK_PDRIVER xdrive                   input or output DSK_PDRIVER
// return: int                                  drive type letter, else 0
//==============================================================================
int get_drive_type (char *xtype, char *xfile, DSK_PDRIVER xdrive)
{
 int res;
 
 // remote drive
 if (! strcmp(xtype, "remote") && xdrive)
    {
     dsk_get_option(xdrive, "DRIVETYPE", &res);
     if (res < 0)
        return 0;
     return res;
    }

#ifdef WIN32
 HANDLE hDrive;
 char device[100];

 if (strcmp(xtype, "ntwdm") != 0)
    return 0;

 snprintf(device, sizeof(device)-1, "\\\\.\\%s", xfile);
 device[sizeof(device)-1] = 0;
 
 hDrive = CreateFile(device, 0, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
 if (hDrive == INVALID_HANDLE_VALUE)
    return 0;

 return get_floppy_type(hDrive);
#endif

#ifdef __linux__
 char drivtyp[17];
 int fd;
 
 if (strcmp(xtype, "floppy") != 0)
    return 0;

 drivtyp[0] = 0;    

 fd = open(xfile, O_RDONLY | O_NDELAY);

 if (fd)
    {
     res = ioctl(fd, FDGETDRVTYP, (void *)drivtyp);
     close(fd);
     if (res < 0)
        return 0;
     return drivtyp[0];  // drive type letter
    }
 return 0;
#endif 

#ifdef __FreeBSD__
 enum fd_drivetype fdtype;
 int fd;

 fd = open(xfile, O_RDONLY | O_NDELAY);
 if (fd)
    {
     res = ioctl(fd, FD_GDTYPE, &fdtype);
     close(fd);
     if (res < 0)
        return 0;
     // Diskette drive type, basically the same as stored in RTC on ISA
     // machines (see /sys/isa/rtc.h), but right-shifted by four bits.
     switch (fdtype)
        {
         case FDT_NONE:
            return 0;
         case FDT_360K:
            return 'd';
         case FDT_12M:
            return 'h';
         case FDT_720K:
            return 'D';
         case FDT_144M:
            return 'H';
         case FDT_288M_1:
            return 'E';
         case FDT_288M:
            return 'E';
         default:
            return 0;
        }
    }
 return 0;
#endif

 // All other builds must return 0 if not implementing this function
 return 0;
}

//==============================================================================
// Initialise the functions module.
//
// For programming information: man termios and man termio (older)
//
//   pass: void
// return: void
//==============================================================================
void functions_init (void)
{
#ifndef WIN32
 // get the current stdin values
 tcgetattr(fileno(stdin), &tOrg);

 // this is what is used when requesting single key presses
 // term.c_cc[VMIN] = 0 prevents blocking.
 memset(&term, 0, sizeof(term));
 term.c_lflag &= ~(ECHO | ICANON);
 term.c_cc[VMIN] = 0;
 term.c_cc[VTIME] = 0;
#endif
}

//==============================================================================
// De-initialise the functions module.
//
//   pass: void
// return: void
//==============================================================================
void functions_deinit (void)
{
#ifndef WIN32
 // re-enable the original stdin values
 tcsetattr(fileno(stdin), TCSANOW, &tOrg);
#endif
}
