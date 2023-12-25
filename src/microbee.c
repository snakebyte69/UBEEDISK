//******************************************************************************
//*                                 uBeeDisk                                   *
//*                                                                            *
//*     A tool for converting disks/images from one to another with auto       *
//*                   detection options for Microbee disks.                    *
//*                                                                            *
//*                             Microbee module                                *
//*                                                                            *
//*                       Copyright (C) 2008-2017 uBee                         *
//******************************************************************************
//
// Select/probe for Microbee disk formats.
//
//==============================================================================
// ChangeLog (most recent entries are at top)
//==============================================================================
// v4.0.0 - 22 January 2017, uBee
// - Added NULLs for the new pskew0, pskew1, GAPs member to all format
//   definitions.
// - Changes in microbee_detect_format() information suggesting to try
//   option --detect=dos if Microbee disk can't be detected.
// - Set all formats to skew_val=2 skew_ofs=1 instead of -1 which was using
//   default values (2 and 1) if options to override not specified.
//
// v3.0.0 - 4 January 2015, uBee
// - Added new fields of sideoffs, call back function fields ssr_cb,
//   rsi_cb, ssd_cb, cif_cb, pss_cb to disk_format_t structure.
// - Replaced GAP values (12, 23) with 0s to force dynamic GAP calculations.
// - Changed Dreamdisk 'ds82' format description changed to 'old entry'.
// - Added Dreamdisk 'dds40' and 'dds80' formats using sideoffs=128.
// - Changes to microbee_detect_format() to not set using constant disk
//   values and use format_set() instead.
//
// v2.3.0 - 30 August 2012, uBee
// - Commented out unassigned variable dsk_err_h1 from microbee_detect_format().
//
// v2.2.0 - 12 May 2012, uBee
// - Changes to allow remote devices to detect formats.
// - Added 'ds8b', 'hs350-80', and 'hs525-80' formats.
//
// v2.1.0 - 7 October 2011, uBee
// - Initial file created from formats.c
// - Added c5h0s0_525 format. (not tested)
// - Changes made to format_set_geometry() to pass additional parameter.

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
#include "microbee.h"
#include "ubeedisk.h"
#include "functions.h"

static int cif_cb (int info);
static int ssr_cb (uint8_t *p, int cyl, int head, int psect);
static int rsi_cb (int cyl, int head, int *xhead, int *xsecsize);
static int ssd_cb (int cyl, int head, int secsize);

//==============================================================================
// structures and variables
//
// microbee_disk_format[]
// ----------------------
// Add any new formats to the help section for the --format option.
//
// The first entry (default) is used to set default values which may then be
// overriden with command line options.  This will be called internally so
// no need to specify it on the command line.  It is important that the GAP
// values in 'default' remain as 0 so that GAP calculation is carried out
// unless set to a non-sero value.
//
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
 FORMAT_HSOFT = 1,
 FORMAT_C5H0S0,
 FORMAT_GAMMA
};

disk_format_t microbee_disk_format[] =
{
 {"default", "Default settings",
  SIDES_ALT, 80, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"user", "User format",
  SIDES_ALT, 80, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"ss40", "Microbee SS DD 40T 5.25\"",
  SIDES_ALT, 40, 1, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"ss80", "Microbee SS DD 80T 3.5\" (CIAB)",
  SIDES_ALT, 80, 1, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },
 
 {"ds40", "Microbee DS DD 40T 5.25\" (SBC)",
  SIDES_ALT, 40, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 1, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"ds80", "Microbee DS DD 80T 3.5\" (Microbee)",
  SIDES_ALT, 80, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 1,  2, 21, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },
 
 {"ds82", "Microbee DS DD 80T 3.5\" (Dreamdisk old entry)",
  SIDES_ALT, 80, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },
 
 {"ds84", "Microbee DS DD 80T 3.5\" (PJB)",
  SIDES_ALT, 80, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"ds8b", "Beeboard DS DD 80T 3.5\"",
  SIDES_ALT, 80, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },
 
 {"ds8x", "Microbee DS DD 80T 3.5\" (non DS80)",
  SIDES_ALT, 80, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"dds40", "Microbee DS DD 40T 5.25\" (Dreamdisk)",
  SIDES_ALT, 40, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 128, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"dds80", "Microbee DS DD 80T 3.5\" (Dreamdisk)",
  SIDES_ALT, 80, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 128, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"hs350", "Microbee Honeysoft 3.5\"",
  SIDES_ALT, 40, 1, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  FORMAT_HSOFT, 0,
  NULL, rsi_cb, ssd_cb, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"hs350-80", "Microbee Honeysoft 3.5\" (80T)",
  SIDES_ALT, 80, 1, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  FORMAT_HSOFT, 0,
  NULL, rsi_cb, ssd_cb, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"hs525", "Microbee Honeysoft 5.25\"",
  SIDES_ALT, 40, 1, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  FORMAT_HSOFT, 0,
  NULL, rsi_cb, ssd_cb, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"hs525-80", "Microbee Honeysoft 5.25\" (80T)",
  SIDES_ALT, 80, 1, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  FORMAT_HSOFT, 0,
  NULL, rsi_cb, ssd_cb, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"c5h0s0_350", "Microbee C5 H0 S0 (Hoards) 3.5\"",
  SIDES_ALT, 80, 1, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  FORMAT_C5H0S0, 0,
  ssr_cb, NULL, ssd_cb, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"c5h0s0_525", "Microbee C5 H0 S0 (Hoards) 5.25\"",
  SIDES_ALT, 40, 2, 10, 1,  512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  FORMAT_C5H0S0, 0,
  ssr_cb, NULL, ssd_cb, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"gamma", "Microbee Gamma DS DD 80T 5x1024 b/s 3.5\"",
  SIDES_ALT, 80, 2,  5, 1, 1024, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1,  1, 0,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"teac", "Microbee TEAC HDD 10MB 306 cylinders, 4 heads, 17 sect/track",
  SIDES_ALT, 306, 4, 17, 1, 512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
  NULL, NULL, ""
 },

 {"mscribe", "Microbee Miniscribe HDD 10MB 612 cylinders, 2 heads, 17 s/t",
  SIDES_ALT, 612, 2, 17, 1, 512, RATE_SD,  0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, cif_cb, NULL,
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
 if (xdg.dg_special == FORMAT_HSOFT)
    {
     switch (info)
        {
         case 0 : 
            fprintf(infof, "10x512 and 5x1024\n");
            return 1;
         case 1 :
            fprintf(infof, "512 and 1024\n");
            return 1;
         default :
            return 0;            
        }
    }
 return 0;
}

//==============================================================================
// Skip sector read call-back.
//
// Work around for disks that have copyright protection where a sector does
// not exist and should be skipped.
//
// If this call-back is not implemented the disk_format field for this
// function must be set to a NULL.
//
//   pass: uint8_t *p
//         int cyl
//         int head
//         int psect
// return: int                          0 else 1 if should skip sector
//==============================================================================
static int ssr_cb (uint8_t *p, int cyl, int head, int psect)
{
 // C5H0S0 disk format ? (sectors on C5, H0 are 0,1,2,3,4,5,6,7,8,9)
 if (xdg.dg_special == FORMAT_C5H0S0 && cyl == 5 && head == 0 && psect == 0)
    {
     memset(p, 0, dg.dg_secsize);
     return 1;
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
 // if honeysoft format
 if (xdg.dg_special == FORMAT_HSOFT)
    {
     if (cyl >= 5)
        *xsecsize = 1024;
     else
        *xsecsize = 512;
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
 // if honeysoft format
 if (xdg.dg_special == FORMAT_HSOFT)
    {
     if (secsize != -1)
        dg.dg_secsize = secsize;
     else
        {
         if (cyl >= 5)
            dg.dg_secsize = 1024;
         else
            dg.dg_secsize = 512;
        }
     if (dg.dg_secsize == 1024)
        dg.dg_sectors = 5;
     else
        dg.dg_sectors = 10;

     return 1;
    }
    
 // if C5H0S0 disk format
 if (xdg.dg_special == FORMAT_C5H0S0)
    {
     if (cyl == 5 && head == 0)
        dg.dg_secbase = 0;
     else
        dg.dg_secbase = 1;

     return 1;
    }

 return 0;
}

//==============================================================================
// Detect Microbee disk/image format.
//
// This process uses a function call to return all the sectors found on a
// track.  The 1st sector number found may not start with 1 and some early
// sectors may be missed.
//
// The testing process is fairly generous on the first number tested for
// (range 1-3, 21-23 allowed) but the last sector number must be found.
//
// The Honeysoft formats appear to be the same for 3.5" and 5.25" disks, only
// uses side 0 with cylinders 0-4 being 10x512 sectors and cylinders 5-39
// using 5x1024 sectors.
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
int microbee_detect_format (void)
{
 dsk_err_t dsk_err_h0 = DSK_ERR_OK;
// dsk_err_t dsk_err_h1 = DSK_ERR_OK;
 dsk_err_t dsk_err_hx = DSK_ERR_OK; 

 dsk_psect_t count_h0;
 dsk_psect_t count_h1;
 dsk_psect_t count_hx;

 int modify_cylinders = 0;

 int size_h0;
 int size_h1;
 int size_hx; 
 
 int t5h0_sec_f;
 int t5h0_sec_l;
 int t5h1_sec_f;
 int t5h1_sec_l;
 int txhx_sec_f;
 int txhx_sec_l;
 
 int sizeok_h0 = 1;
 int sizeok_h1 = 1;
 int sizeok_hx = 1;

 int f = -1;
 int drive_type = 0;

 // establish the drive type detection required
 switch (disk.detect)
    {
     case det_mbee : // let the host/remote determine the type

        if (strcmp("remote", disk.itype) != 0 &&
#ifdef WIN32
        strcmp("ntwdm", disk.itype) != 0)
#else
        strcmp("floppy", disk.itype) != 0)
#endif
           {
            printf("'mbee' detection is only for floppy drive types,"
                   " try using 'mbee3' or 'mbee5'\n");            
            return -1;
           } 
        if ((disk.idrive_type == 'd') || (disk.idrive_type == 'h'))
           drive_type = 5;
        else
           drive_type = 3;
        break;
     case det_mbee3 : 
        drive_type = 3;
        break;
     case det_mbee5 : 
        drive_type = 5;
        break;
    }

 // values common to all microbee formats (DS8x and DS40)
 if (drive_type == 3)
    f = format_set("ds84", 0);
 else   
    f = format_set("ds40", 0);

 // read cylinder 5, side 0 of the disk
 dsk_err_h0 = read_sector_ids(5, 0, 512, &sizeok_h0, &size_h0,
 &count_h0, &t5h0_sec_f, &t5h0_sec_l);

 // exit if the driver could not support the required functions
 if (dsk_err_h0 == DSK_ERR_NOTIMPL)
    return -1;

 // if nothing on T5 H0 then bad, non formatted, no disk, wrong data rate ?
 if (dsk_err_h0 != DSK_ERR_OK)
    {
     printf("No disk, non formatted, bad disk, or wrong data rate ???\n");
     return -1;
    }

 // read cylinder 5, side 1 of the disk
 // dsk_err_h1 =
 read_sector_ids(5, 1, 512, &sizeok_h1, &size_h1,
 &count_h1, &t5h1_sec_f, &t5h1_sec_l);

 for (;;)
    {
     // check for Microbee DS80 format
     if ((t5h0_sec_f > 20) && (t5h0_sec_f < 24) && (t5h0_sec_l == 30) &&
         (t5h1_sec_f > 20) && (t5h1_sec_f < 24) && (t5h1_sec_l == 30) &&
         (sizeok_h0) && (sizeok_h1) && (drive_type == 3))
        {
         f = format_set("ds80", 0);
         break;
        } 

     // check for Microbee DS82, DS84 format
     if ((t5h0_sec_f > 0) && (t5h0_sec_f < 4) && (t5h0_sec_l == 10) &&
         (t5h1_sec_f > 0) && (t5h1_sec_f < 4) && (t5h1_sec_l == 10) &&
         (sizeok_h0) && (sizeok_h1) && (drive_type == 3))
        {
         f = format_set("ds8x", 0);
         break;
        }

     // check for Microbee SS80 format
     if ((t5h0_sec_f > 0) && (t5h0_sec_f < 4) && (t5h0_sec_l == 10) &&
         (sizeok_h0) && (drive_type == 3))
        {
         f = format_set("ss80", 0);
         break;
        }

     // check for Microbee DS40 format
     if ((t5h0_sec_f > 0) && (t5h0_sec_f < 4) && (t5h0_sec_l == 10) &&
         (t5h1_sec_f > 0) && (t5h1_sec_f < 4) && (t5h1_sec_l == 10) &&
         (sizeok_h0) && (sizeok_h1) && (drive_type == 5))
        {
         f = format_set("ds40", 0);
         break;
        }

     // check for Microbee SS40 format
     if ((t5h0_sec_f > 0) && (t5h0_sec_f < 4) && (t5h0_sec_l == 10) &&
         (sizeok_h0) && (drive_type == 5))
        {
         f = format_set("ss40", 0);
         break;
        }

     // check for Microbee Honeysoft 3.5" format
     if (((size_h0 == 1024) || (size_h1 == 1024)) && (drive_type == 3))
        {
         f = format_set("hs350", 0);
         // read cylinder 40, side 0 of the disk
         dsk_err_hx = read_sector_ids(40, 0, 512, &sizeok_hx, &size_hx,
         &count_hx, &txhx_sec_f, &txhx_sec_l);
         if ((dsk_err_hx == DSK_ERR_OK) && (txhx_sec_f < 6) &&
            (size_hx == 1024))
            modify_cylinders = 80;
         break;
        }
        
     // check for Microbee Honeysoft 5.25" format (will always be SS ?)
     if ((size_h0 == 1024) && (drive_type == 5))
        {
         f = format_set("hs525", 0);
         break;
        }

     // check for C5H0S0 Microbee (Hoards) 3.5" format
     if ((t5h0_sec_f == 0) && (sizeok_h0) && (drive_type == 3))
        {
         f = format_set("c5h0s0_350", 0);
         break;
        }

     // check for C5H0S0 Microbee (Hoards) 5.25" format
     if ((t5h0_sec_f == 0) && (sizeok_h0) && (drive_type == 5))
        {
         f = format_set("c5h0s0_525", 0);
         break;
        }

     // no format detected    
     break;
    }   

 // show the format if one was detected
 if (f != -1)
    {
     if (modify_cylinders)
        dg.dg_cylinders = modify_cylinders;
     if (disk.verbose)
        printf("%s: %s\n", microbee_disk_format[f].name,
               microbee_disk_format[f].desc);
     return 0;
    } 

 // if we got here then the format was not detected, report what was found
 printf(
 "\n"
 "Microbee disk type could not be detected. This does not necessarily mean\n"
 "the disk is bad or non-recoverable, the detection process used may not have\n"
 "worked for various reasons.\n\n"
 "If you know what format the disk is then try --format=name option instead\n"
 "or use the --disk=scan option to check.  It may also be a DOS type disk so\n"
 "a --detect=dos may be useful to determine the structure.\n\n"
 "Setting the correct data rate (--datarate=dd) and using double stepping\n"
 "(--idstep=on) is also important if 5.25\" disks are being used in 5.25\"\n"
 "HD 80T 1.2Mb drives.\n\n"
 "The following information was gathered about the disk:"
 "\n\n"
 );

 printf(
 "Cylinder 5, Head 0: Sectors/track: %d  Size: %d  First: %d  Last: %d\n",
 count_h0, size_h0, t5h0_sec_f, t5h0_sec_l);
 printf(
 "Cylinder 5, Head 1: Sectors/track: %d  Size: %d  First: %d  Last: %d\n",
 count_h1, size_h1, t5h1_sec_f, t5h1_sec_l);

 strcpy(xdg.dg_format_name, "");
 return -1;
}

//==============================================================================
// List formats available in this module.
//
//   pass: void
// return: void
//==============================================================================
void microbee_list_formats (void)
{
}
