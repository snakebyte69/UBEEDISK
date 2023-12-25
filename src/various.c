//******************************************************************************
//*                                 uBeeDisk                                   *
//*                                                                            *
//*     A tool for converting disks/images from one to another with auto       *
//*                   detection options for Microbee disks.                    *
//*                                                                            *
//*                              Various module                                *
//*                                                                            *
//*                       Copyright (C) 2008-2017 uBee                         *
//******************************************************************************
//
// Select/probe for various disk formats.
//
// s80cpm1
// -------
// Super-80 CP/M format.  80T, DS, DD, 512 b/s, 10 s/t (sectors 1-10 and 11-20)
// This format uses physically skewed sectors.
//
//==============================================================================
// ChangeLog (most recent entries are at top)
//==============================================================================
// v4.0.0 - 22 January 2017, uBee
// - Added NULLs for the new pskew0, pskew1, GAPs member to all format
//   definitions. Replaced some 'pss_cb' where skewing was simple.
// - Added 'datamax8000sd' format with a skew of 1,0.
// - Added 'lbb_40dsdd' format with a skew of 1,0 (uses physical skewing).
// - Added 'lbb_77dshd' format with a skew of 3,1 and disable write caching.
// - Added special low level 'erase-40' and 'erase-80' track erase formats.
// - Set all formats to skew_val=2 skew_ofs=1 instead of -1 which was using
//   default values (2 and 1) if options to override not specified.
//
// v3.1.0 - 8 July 2015, uBee
// - Removed the old 'knight80' entry (use 'k80dsdd1024')
// - Added Aussie Byte SME Knight 5.25" DS DD 80T 80x5x1024 b/s 'k80dsdd1024'
// - Added Aussie Byte SME Knight 5.25" SS SD 80T 80x18x128 b/s 'k80sssd128'
// - Added Aussie Byte SME Knight 5.25" DS DD 80T 80x10x512 b/s 'k80dsdd512'
//
// v3.0.0 - 19 December 2014, uBee
// - Initial file created from applix.c
//
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
#include "various.h"
#include "ubeedisk.h"
#include "functions.h"

static int cif_cb (int info);
static int rsi_cb (int cyl, int head, int *xhead, int *xsecsize);
static int ssd_cb (int cyl, int head, int secsize);
static int pss_cb (int cyl, int head, int lsect);

//==============================================================================
// structures and variables
//
// various_disk_format[]
// ---------------------
// The skew_val and skew_ofs value should be set to -1 if the default values
// should be used.
//
// Note: The Amust Compak system tracks are smaller than the data tracks so
// DSK format can not be used (produces error).  Must use EDSK format or
// other formats that support this.
//==============================================================================
static int pskew_compak [] = {1,9,2,10,3,11,4,12,5,13,6,14,7,15,8,16};

static int pskew_portapak [] = {1,3,5,7,9,2,4,6,8,10};

static int pskew_s80_cpm1_0 [] = {1,  8, 5, 2, 9, 6, 3,10, 7, 4};
static int pskew_s80_cpm1_1 [] = {11,18,15,12,19,16,13,20,17,14};

static int pskew_lbb40 [] = {1,6,2,7,3,8,4,9,5};

// name description
// sidedness cylinders heads sectors secbase secsize datarate rwgap fmtgap fm
// nomulti noskip sideoffs side1as0 secbase2_c secbase2_s skew_val skew_ofs
// special attributes
// ssr_cb rsi_cb ssd_cb cif_cb pss_cb
// pskew0 pskew1 gap

// special format values
enum
{
 FORMAT_S80CPM1 = 1,
 FORMAT_COMPAK,
 FORMAT_PORTAPAK,
};

disk_format_t various_disk_format[] =
{
 {"cal80", "Calibrate disk DD 80T 80x10x256 b/s base=100 side=10/11",
  SIDES_ALT, 80, 2,  10, 100, 256, RATE_DD, 0, 0, 0,
  1, 1, 10, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

{"erase-40", "40T DS DD Low level tracks ERASE with 0s (no address marks, etc)",
  SIDES_ALT, 40, 2,  10, 100, 256, RATE_DD, 0, 0, 0,
  1, 1, 10, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

{"erase-80", "80T DS DD Low level tracks ERASE with 0s (no address marks, etc)",
  SIDES_ALT, 80, 2,  10, 100, 256, RATE_DD, 0, 0, 0,
  1, 1, 10, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"test1", "Small 3.5\" 1.44MB DS DD 2x10x512 drive disk image for testing purposes",
  SIDES_ALT, 2, 2,  10, 1, 512, RATE_SD, 0, 0, 0,
  1, 1, 10, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"dsdd-80x10x512", "DS DD 80x10x512 b/s",
  SIDES_ALT, 80, 2, 10, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"dsdd-40x10x512", "DS DD 40x10x512 b/s",
  SIDES_ALT, 40, 2, 10, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"dsdd-80x5x1024", "DS DD 80x5x1024 b/s",
  SIDES_ALT, 80, 2,  5, 1, 1024, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"ssdd-80x5x1024", "SS DD 80x5x1024 b/s",
  SIDES_ALT, 80, 1,  5, 1, 1024, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"compak", "Amust Compak 5.25\" DS DD 80T 1x16x256, 79x5x1024 b/s",
  SIDES_ALT, 80, 2,  16, 1, 256, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  FORMAT_COMPAK, ATTR_VAR_TRACK_SIZE,
  NULL, rsi_cb, ssd_cb, cif_cb, pss_cb,
  NULL, NULL, ""
 },

 {"datamax8000sd", "Datamax 8\" SS SD 77T, 77x26x128 b/s",
  SIDES_ALT, 77, 1,  26, 1, 128, RATE_HD, 0, 0, 1,
  1, 1, 128, 0, -1,  1, 1, 0,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"excalibur4", "Excalibur 5.25\" DS DD 80T 80x5x1024 b/s",
  SIDES_ALT, 80, 2,  5, 1, 1024, RATE_DD, 0, 0, 0,
  1, 1, 128, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"k80dsdd1024", "Aussie Byte SME Knight 5.25\" DS DD 80T 80x5x1024 b/s",
  SIDES_ALT, 80, 2,  5, 1, 1024, RATE_DD, 0, 0, 0,
  1, 1, 128, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"k80dsdd512", "Aussie Byte SME Knight 5.25\" DS DD 80T 80x10x512 b/s",
  SIDES_ALT, 80, 2, 10, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 128, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"k80sssd128", "Aussie Byte SME Knight 5.25\" SS SD 80T 80x18x128 b/s",
  SIDES_ALT, 80, 1, 18, 1, 128, RATE_DD, 0, 0, 1,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"lbb_40dsdd", "Little Big Board 5.25\" DS DD 40T 40x9x512 b/s",
  SIDES_ALT, 40, 2, 9, 1,  512, RATE_DD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 1, 0,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  pskew_lbb40, pskew_lbb40, ""
 },

 {"lbb_77dshd", "Little Big Board 5.25\" DS HD 77T 77x17x512 b/s",
  SIDES_ALT, 77, 2, 17, 1,  512, RATE_HD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 3, 1,
  0, ATTR_CACHEW_OFF,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, "g1:16,s1:12, g2:16,s2:8, g3:22,s3:12, g4:16"
 },

 {"portapak", "PortaPak 5.25\" DS DD 80T 80x10x512 b/s",
  SIDES_ALT, 80, 2, 10, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  FORMAT_PORTAPAK, 0,
  NULL, NULL, NULL, NULL, pss_cb,
  NULL, NULL, ""
 },

 {"s80cpm1", "Super-80 CP/M 80T DS DD 2x10x512",
  SIDES_ALT, 80, 2,  10, 1, 512, RATE_SD, 0, 0, 0,
  1, 1, 0, 0, -1,  1,  6, -1,
  FORMAT_S80CPM1, 0,
  NULL, NULL, ssd_cb, NULL, NULL,
  pskew_s80_cpm1_0, pskew_s80_cpm1_1, ""
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

//==============================================================================
// Create info file call-back.
//
// Outputs file information string for some formats. The string output
// depends on the requested info value.
//
// If this call-back is not implemented the disk_format field for this
// function must be set to a NULL or return 0 if no output made.
//
//   pass: int info
// return: int                          0 if nothing output, else 1
//==============================================================================
static int cif_cb (int info)
{
 if (xdg.dg_special == FORMAT_COMPAK)
    {
     switch (info)
        {
         case 0 :
            fprintf(infof, "16x256 and 5x1024\n");
            return 1;
         case 1 :
            fprintf(infof, "256 and 1024\n");
            return 1;
         default :
            return 0;
        }
    }
 return 0;
}

//==============================================================================
// Read sector ID call-back.
//
// Set the ID sector size for disk formats that do not support reading
// sector IDs.
//
// If this call-back is not implemented the disk_format field for this
// function must be set to a NULL.
//
//   pass: int cyl
//         int head
//         int *xhead
//         int *xsecsize
// return: int                          1 if acted on
//==============================================================================
static int rsi_cb (int cyl, int head, int *xhead, int *xsecsize)
{
 // if Compak format
 if (xdg.dg_special == FORMAT_COMPAK)
    {
     if (cyl >= 1)
        *xsecsize = 1024;
     else
        *xsecsize = 256;
     *xhead = head + xdg.dg_sideoffs;
     return 1;
    }
 return 0;
}

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
 // if Super-80 80T CP/M disk format
 if (xdg.dg_special == FORMAT_S80CPM1)
    {
     if (head == 0)
        dg.dg_secbase = 1;
     else
        dg.dg_secbase = 11;
     return 1;
    };

 // if Compak format
 if (xdg.dg_special == FORMAT_COMPAK)
    {
     if (secsize != -1)
        dg.dg_secsize = secsize;
     else
        {
         if (cyl >= 1)
            dg.dg_secsize = 1024;
         else
            dg.dg_secsize = 256;
        }
     if (dg.dg_secsize == 1024)
        dg.dg_sectors = 5;
     else
        dg.dg_sectors = 16;
     return 1;        
    }

 return 0;
}

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
     case FORMAT_COMPAK :
        if (cyl == 0)
           return pskew_compak[lsect];
        return -1;
     case FORMAT_PORTAPAK :
        if (cyl == 0 || (cyl == 1 && head == 0))
           return pskew_portapak[lsect];
        return -1;
     default :
        return -1;
    }
}

//==============================================================================
// Detect various disk/image formats.
//
// This function currently does nothing.
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
int various_detect_format (void)
{
 printf("\nVarious disk detection not implemented.\n");
 strcpy(xdg.dg_format_name, "");
 return -1;
}
