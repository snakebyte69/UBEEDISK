//******************************************************************************
//*                                 uBeeDisk                                   *
//*                                                                            *
//*     A tool for converting disks/images from one to another with auto       *
//*                   detection options for Microbee disks.                    *
//*                                                                            *
//*                              Applix module                                 *
//*                                                                            *
//*                       Copyright (C) 2008-2017 uBee                         *
//******************************************************************************
//
// Select/probe for Applix disk formats.
//
//==============================================================================
// ChangeLog (most recent entries are at top)
//==============================================================================
// v4.0.0 - 22 January 2017, uBee
// - Added 'applix_pskew_0' and 'applix_pskew_1' skew pointers for the new
//   pskew0, pskew1, and GAPs member to all formats.  Removed the old
//   physcial 'pss_cb' skew method.
// - Changed "char buffer" to "uint8_t buffer".
// 
// v3.0.0 - 4 January 2015, uBee
// - Replaced GAP values (40, 120) with 0s to force dynamic GAP calculations.
// - Added new fields of sideoffs to disk_format_t structure.
// - Removed FLAGS_APPLIX and replaced with FORMAT_APPLIX value.
// - Changes to applix_detect_format() to not set using constant disk values
//   and use format_set() instead.
//
// v2.1.0 - 8 October 2011, uBee
// - Initial file created from formats.c

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

#include "format.h"
#include "applix.h"
#include "ubeedisk.h"
#include "functions.h"

#if 0
static int pss_cb (int cyl, int head, int lsect);
#endif

//==============================================================================
// structures and variables
//
// applix_disk_format[]
// --------------------
// The skew_val and skew_ofs value should be set to -1 if the default values
// should be used.
//==============================================================================
static int applix_pskew_0[] = {4,2,5,3,1};
static int applix_pskew_1[] = {1,4,2,5,3};

// name description
// sidedness cylinders heads sectors secbase secsize datarate rwgap fmtgap fm
// nomulti noskip sideoffs side1as0 secbase2_c secbase2_s skew_val skew_ofs
// special attributes
// ssr_cb rsi_cb ssd_cb cif_cb pss_cb
// pskew0 pskew1 gap

disk_format_t applix_disk_format[] =
{
 {"applix80", "Applix 1616/OS DS DD 80T 5x1024 b/s 3.5\"",
  SIDES_ALT, 80, 2,  5, 1, 1024, RATE_SD, 0, 0, 0,
  1, 1, 0, 0, -1,  1,  1, 0,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  applix_pskew_0, applix_pskew_1, ""
 },

 {"applix81", "Applix 1616/OS DS DD 81T 5x1024 b/s 3.5\"",
  SIDES_ALT, 81, 2,  5, 1, 1024, RATE_SD, 0, 0, 0,
  1, 1, 0, 0, -1,  1,  1, 0,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  applix_pskew_0, applix_pskew_1, ""
 },

 {"applix82", "Applix 1616/OS DS DD 82T 5x1024 b/s 3.5\"",
  SIDES_ALT, 82, 2,  5, 1, 1024, RATE_SD, 0, 0, 0,
  1, 1, 0, 0, -1,  1,  1, 0,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  applix_pskew_0, applix_pskew_1, ""
 },

 {"applix83", "Applix 1616/OS DS DD 83T 5x1024 b/s 3.5\"",
  SIDES_ALT, 83, 2,  5, 1, 1024, RATE_SD, 0, 0, 0,
  1, 1, 0, 0, -1,  1,  1, 0,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  applix_pskew_0, applix_pskew_1, ""
 },

 {"", "", 
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0, 0, 0, 0, 0,  0, -1, -1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 }
};

extern char *no_dsk_ptrackids[];
extern FILE *infof;
extern DSK_PDRIVER idrive;
extern DSK_GEOMETRY dg;
extern disk_t disk;
extern xdg_t xdg;

#if 0
//==============================================================================
// Get physical sector skew value call-back.
//
// If this call-back is not implemented the disk_format field for this
// function must be set to a NULL or return -1 if not acted on.
//
//   pass: int cyl                      cylinder number
//         int head                     head number
//         int lsect                    logical sector number
// return: int                          -1 if not acted on, else sector value
//==============================================================================
static int pss_cb (int cyl, int head, int lsect)
{
 // if physical skewing is being used
 switch (xdg.dg_special)
    {
     default :
        return -1;
    }
}
#endif

//==============================================================================
// Detect Applix disk/image format.
//
// The first 2 bytes of the disk image records the number of 1024 byte
// blocks on the disk.  This is used to determine how many tracks are on the
// disk so the correct format can be set.
//
// Sector skewing Side 0:
// 42531
//
// Sector skewing Side 1:
// 14253
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
int applix_detect_format (void)
{
 uint8_t buffer[1024];
 int blocks;
 int f;
 
 // values common to all Applix formats?
 f = format_set("applix80", 0);

 // read first sector from T0, H0
 if (read_physical_sector(0, 0, 0, 0, 1, buffer) != DSK_ERR_OK)
    {
     printf("\nApplix disk detection not able to read sector.\n");
     strcpy(xdg.dg_format_name, "");
     return -1;
    }

 // number of blocks on disk (big endian value to host format)
 blocks = buffer[0] << 8 | buffer[1];

 switch (blocks)
    {
     case 800 :
        f = format_set("applix80", 0);
        break;
     case 810 :
        f = format_set("applix81", 0);
        break;
     case 820 :
        f = format_set("applix82", 0);
        break;
     case 830 :
        f = format_set("applix83", 0);
        break;
      default :
         strcpy(xdg.dg_format_name, "");
         printf("\nApplix block size (%d) not recognised.\n", blocks);
         return -1;
    }

 // should not be any errors
 if (f == -1)
    return -1;

 // report the format that has been set
 if (disk.verbose)
    printf("%s: %s\n", applix_disk_format[f].name,
           applix_disk_format[f].desc);

 return 0;
}
