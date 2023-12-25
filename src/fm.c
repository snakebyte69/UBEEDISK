//******************************************************************************
//*                                 uBeeDisk                                   *
//*                                                                            *
//*     A tool for converting disks/images from one to another with auto       *
//*                   detection options for Microbee disks.                    *
//*                                                                            *
//*                                fm module                                   *
//*                                                                            *
//*                       Copyright (C) 2008-2017 uBee                         *
//******************************************************************************
//
// Select/probe for FM disk formats (typically 8" disks).
//
// 60
// --
// 60 Sector - 8" 77T DS DD 30 s/t 128 b/s
// This format numbers sectors on side 0 as 1-30 and on side 1 as 31-60. Side
// 1 also uses side 0 in the sector ID headers.
//
// This disk format is basically a singled sided 77T disk with 60 sectors
// per track (but continues on side 1 for sectors 31-60).
//
// 3030
// ----
// 30/30 Sector - 8" 77T SS DD 30 s/t 128 b/s
//
// This is a format used on each side of a double sided disk. Sectors are
// numbered 1-30 on each side, Side 1 also uses side 0 in the sector ID
// headers.  This is NOT a 'flipper' disk. See [fromside1].
//
// To image both sides of the disk with 2 passes:
// ubeedisk 3030 --if=x: --of=disk_3030_001a.raw
// ubeedisk 3030 --if=x: --of=disk_3030_001b.raw fromside1
//
// Alternatively the disk could be read with 1 pass by using an override
// --heads=2 option but will require some simple software to de-interleave
// the tracks to make 2 disk images from the one.
//
//==============================================================================
// ChangeLog (most recent entries are at top)
//==============================================================================
// v4.0.0 - 26 January 2017, uBee
// - Added 'ibm3740ds' format with a skew of 1,0 and 'nshs' format.
// - Added NULLs for the new pskew0, pskew1, GAPs member to all format
//   definitions.
// - Set all formats to skew_val=2 skew_ofs=1 instead of -1 which was using
//   default values (2 and 1) if options to override not specified.
//
// v3.0.0 - 9 January 2015, uBee
// - Added 'ds40s' and 'ss40s' 40x18x128 b/s 5.25" SD formats.
// - Added new fields of sideoffs, call back function fields ssr_cb,
//   rsi_cb, ssd_cb, cif_cb, pss_cb to disk_format_t structure.
// - Changes to fm_detect_format() to not set using constant disk values and
//   use format_set() instead.
//
// v2.3.0 - 30 August 2012, uBee
// - Commented out assigning unused variable dsk_err_h1 from fm_detect_format().
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
#include "fm.h"
#include "ubeedisk.h"
#include "functions.h"

static int ssd_cb (int cyl, int head, int secsize);

//==============================================================================
// structures and variables
//
// fm_disk_format[]
// ----------------
// The skew_val and skew_ofs value should be set to -1 if the default values
// should be used.
//==============================================================================
// name description
// sidedness cylinders heads sectors secbase secsize datarate rwgap fmtgap fm
// nomulti noskip sideoffs side1as0 secbase2_c secbase2_s skew_val skew_ofs
// special attributes
// ssr_cb rsi_cb ssd_cb cif_cb pss_cb
// pskew0 pskew1 gap

// special format values
enum
{
 FORMAT_60 = 1
};

disk_format_t fm_disk_format[] =
{
 {"ibm3740", "8\" 77T SS DD 77x26x128 b/s",
  SIDES_ALT, 77, 1, 26, 1,  128, RATE_HD,  0, 0, 1,
  1, 1, 0, 0, -1, 1,  1, 0,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"ibm3740ds", "8\" 77T DS SD 77x26x128 b/s",
  SIDES_ALT, 77, 2, 26, 1,  128, RATE_HD,  0, 0, 1,
  1, 1, 0, 0, -1, 1,  1, 0,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"gbss77",  "8\" 77T SS HD 77x9x512 b/s",
  SIDES_ALT, 77, 1,  9, 1,  512, RATE_HD,  0, 0, 1,
  1, 1, 0, 0, -1, 1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"3030", "30 Sector - 8\" 77T DS DD 77x30x128 b/s",
  SIDES_ALT, 77, 1, 30, 1,  128, RATE_HD,  0, 0, 1,
  1, 1, 0, 0, -1, 1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"60", "60 Sector - 8\" 77T DS DD 77x30x128 b/s",
  SIDES_ALT, 77, 2, 30, 1,  128, RATE_HD,  0, 0, 1,
  1, 1, 0, 0, -1,  1, 2, 1,
  FORMAT_60, 0,
  NULL, NULL, ssd_cb, NULL, NULL,
  NULL, NULL, ""
 },

 {"ds40s", "5.25\" 40T DS SD 40x18x128 b/s",
  SIDES_ALT, 40, 2, 18, 1,  128, RATE_DD, 0, 0, 1,
  1, 1, 0, 1, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"ss40s", "5.25\" 40T SS SD 40x18x128 b/s",
  SIDES_ALT, 40, 1, 18, 1,  128, RATE_DD, 0, 0, 1,
  1, 1, 0, 1, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"nshs", "North Star Horizon 5.25\" 35T SS SD 35x10x256 b/s",
  SIDES_ALT, 35, 1, 10, 1,  256, RATE_SD, 0, 0, 1,
  1, 1, 0, 1, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },
   
 {"", "", 
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0, 0, 0, 0, 0,  0, 2, 1,
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

//==============================================================================
// Set special disk call-back.
//
// Set values required for special disk formats.
//
// If this call-back is not implemented the disk_format field for this
// function must be set to a NULL or return 0 if not acted on.
//
//   pass: int cyl
//         int head
//         int secsize
// return: int                          0 if not acted on, else 1
//==============================================================================
static int ssd_cb (int cyl, int head, int secsize)
{
 // if 8" 60 sector disk format
 if (xdg.dg_special == FORMAT_60)
    {
     if (head == 0)
        dg.dg_secbase = 1;
     else
        dg.dg_secbase = 31;
     return 1;
    }

 return 0;
}

//==============================================================================
// Detect Other disk/image format.
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
int fm_detect_format (void)
{
 dsk_err_t dsk_err_h0 = DSK_ERR_OK;
// dsk_err_t dsk_err_h1 = DSK_ERR_OK;

 dsk_psect_t count_h0;
 dsk_psect_t count_h1;

 int size_h0;
 int size_h1;
 
 int t0h0_sec_f = 0;
 int t0h0_sec_l = 0;
 int t0h1_sec_f = 0;
 int t0h1_sec_l = 0;
 
 int sizeok_h0;
 int sizeok_h1;

 int f = -1; 
 int drive_type = 0;

 // establish the drive type detection required
 if ((disk.idrive_type == 'd') || (disk.idrive_type == 'h'))
    drive_type = 5;
 else
    drive_type = 3;

 // set some values depending on the drive type
 if (drive_type == 5)
    f = format_set("ibm3740", 0);
 else   
    {
     printf("\nNo FM formats for 3.5\" drives implemented.\n");
     return -1;
    } 

 // read cylinder 0, side 0 of the disk
 dsk_err_h0 = read_sector_ids(0, 0, 128, &sizeok_h0, &size_h0,
 &count_h0, &t0h0_sec_f, &t0h0_sec_l);

 // read cylinder 0, side 1 of the disk
// dsk_err_h1 = 
 read_sector_ids(0, 1, 128, &sizeok_h1, &size_h1,
 &count_h1, &t0h1_sec_f, &t0h1_sec_l);

 // if nothing on T0 H0 then bad, non formatted, no disk, wrong data rate ?
 if (dsk_err_h0 != DSK_ERR_OK)
    {
     printf("No disk, non formatted, bad disk, or wrong data rate ???\n");
     return -1;
    }

 // check values
 for (;;)
    {
     if (size_h0 == 128 && count_h0 == 26)
        {
         f = format_set("ibm3740", 0);
         break;
        } 
     if (size_h0 == 512 && count_h0 == 9)
        {
         f = format_set("gbss77", 0);
         break;
        } 
     if (size_h0 == 128 && count_h0 == 30 && size_h1 == 128 && t0h1_sec_f < 31)
        {
         f = format_set("3030", 0);
         break;
        } 
     if (size_h0 == 128 && count_h0 == 30 && size_h1 == 128 && t0h1_sec_f > 30)
        {
         f = format_set("60", 0);
         break;
        } 

     // no format detected
     break;
    }
     
 // set the format
 if (f != -1)
    {
     // report the format that has been set
     if (disk.verbose)
        printf("%s: %s\n", fm_disk_format[f].name, fm_disk_format[f].desc);
     return 0;
    }

 printf("\nFormat not recognised.\n");
 printf(
 "Cylinder 0, Head 0: Sectors/track: %d  Size: %d  First: %d  Last: %d\n",
 count_h0, size_h0, t0h0_sec_f, t0h0_sec_l);
 printf(
 "Cylinder 0, Head 1: Sectors/track: %d  Size: %d  First: %d  Last: %d\n",
 count_h1, size_h1, t0h1_sec_f, t0h1_sec_l);

 strcpy(xdg.dg_format_name, "");
 return -1;
}
