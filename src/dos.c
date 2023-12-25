//******************************************************************************
//*                                 uBeeDisk                                   *
//*                                                                            *
//*     A tool for converting disks/images from one to another with auto       *
//*                   detection options for Microbee disks.                    *
//*                                                                            *
//*                                DOS module                                  *
//*                                                                            *
//*                       Copyright (C) 2008-2017 uBee                         *
//******************************************************************************
//
// Select/probe for various DOS disk formats.  This includes MSDOS, Atari
// TOS, MSX-DOS and other system disks that share the same disk structure.
//
// The detection is not perfect as it appears some DOS disks don't follow
// various documented structure.  i.e.  5.25" 8 s/t MSDOS disks formatted on
// a 6.22 DOS system enters information into BPB but with a signature of
// 0x0000 instead of 0xaa55.  Because of this the wrong information might be
// displayed about the disk but the vital information for tracks, sides,
// sectors per track and sector size should be correct.
//
// References:
// http://alumnus.caltech.edu/~pje/dosfiles.html
// http://info-coach.fr/atari/software/FD-Soft.php (serial is really 3 bytes)
// https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system
//
// The Sector size, number of sectors/track, tracks, density should be in
// the first sector of the disk.  The FAT(s) and other information is
// ignored as we are only interested in the physical layout of the disk
// here.  The first FAT is read if the disk looks like an 8 sector/track
// disk to determine the layout from the media descriptor byte.
//
// Tools such as Unices 'mtools' can be used on RAW disk images. It is
// recommended to have the file '.mtoolsrc' file in the user's home
// directory which contain the following line: "mtools_skip_check=1" which
// will ignore unkown media formats which should allow all Atari ST and
// MSX-DOS disk images to be accessed.  Then use one of the mtools with "-i
// imagename.raw" to access it.
//
// Use --detect=dos to use the auto detection in this module.  It should
// take care of the rest.  It is also recommended to use the filename
// substitution characters for output images.
//
//==============================================================================
// ChangeLog (most recent entries are at top)
//==============================================================================
// v4.0.0 - 22 January 2017, uBee
// - Initial file created from various.c
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
#include "dos.h"
#include "ubeedisk.h"
#include "functions.h"

//==============================================================================
// structures and variables
//==============================================================================
// name description
// sidedness cylinders heads sectors secbase secsize datarate rwgap fmtgap fm
// nomulti noskip sideoffs side1as0 secbase2_c secbase2_s skew_val skew_ofs
// special attributes
// ssr_cb rsi_cb ssd_cb cif_cb pss_cb
// pskew0 pskew1 gap

disk_format_t dos_disk_format[] =
{
 {"dos", "DOS disk structure auto detection",
  SIDES_ALT, 80, 2, 9, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"msdos-160kb", "MSDOS 5.25\" 160kb SS DD 40x8x512",
  SIDES_ALT, 40, 1, 8, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"msdos-180kb", "MSDOS 5.25\" 180kb SS DD 40x9x512",
  SIDES_ALT, 40, 1, 9, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"msdos-320kb", "MSDOS 5.25\" 320kb DS DD 40x8x512",
  SIDES_ALT, 40, 2, 8, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"msdos-360kb", "MSDOS 5.25\" 360kb DS DD 40x9x512",
  SIDES_ALT, 40, 2, 9, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"msdos-1200kb", "MSDOS 5.25\" 1200kb DS HD 80x15x512",
  SIDES_ALT, 80, 2, 15, 1, 512, RATE_HD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"msdos-720kb", "MSDOS 3.5\" 720kb DS DD 80x9x512",
  SIDES_ALT, 80, 2, 9, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"msdos-144mb", "MSDOS 3.5\" 1.44mb DS HD 80x18x512",
  SIDES_ALT, 80, 2, 18, 1, 512, RATE_HD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },

 {"atari-ss80", "Atari ST 3.5\" 360kb SS DD 80x9x512",
  SIDES_ALT, 80, 1, 9, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 },
 
 {"atari-ds80", "Atari ST 3.5\" 720kb DS DD 80x9x512",
  SIDES_ALT, 80, 2, 9, 1, 512, RATE_DD, 0, 0, 0,
  1, 1, 0, 0, -1,  1, 2, 1,
  0, 0,
  NULL, NULL, NULL, NULL, NULL,
  NULL, NULL, ""
 }
};

static char *datarates_str[] =
{
 "hd",  // High-density disc (1.4Mb or 1.2Mb)
 "dd",  // Double-density disc in 1.2Mb drive (ie, 360k disc in 1.2Mb drive)
 "sd",  // Double-density disc in 1.4Mb or 720k drive
 "ed"   // RATE_ED Extra-density disc (2.8Mb)
};

static char *dos_names[] =
{
 "dos",
 "atari",
 "msxdos"
};

static char msxdos2_vs[7];

extern char *no_dsk_ptrackids[];
extern FILE *infof;
extern DSK_PDRIVER idrive;
extern DSK_GEOMETRY dg;
extern disk_t disk;
extern xdg_t xdg;
extern dg_opts_t dg_opts;

//==============================================================================
// Detect failure message.
//
//   pass: void
// return: void
//==============================================================================
static void detect_failed (void)
{
 printf(
 "\n"
 "Disk type could not be detected. This does not necessarily mean\n"
 "the disk is bad or non-recoverable, the detection process used may not have\n"
 "worked for various reasons.\n\n"
 );
}

//==============================================================================
// Disk information.
//
// Reports disk information from the DOS disk boot sector.  Some of the
// information varies depending on the actual DOS format but the BIOS
// Parameter Block (BPB) is the essentially the same for all.
//
//   pass: dos_bootstrap_u *boot
//         int diskx                 dos_ms, dos_atari, dos_msx
// return: void
//==============================================================================
static void detect_information (dos_bootstrap_u *boot, int diskx)
{
 int i;

 if (disk.verbose == 0)
    return;
 
 if (diskx == dos_ms)
    {
     printf(
     "DOS Boot disk sector information:\n"
     "OEM name:            ");
     for (i = 0; i < 8; i++)
        {
         if (boot->dos.oem_name[i] >= ' ')
            putchar(boot->dos.oem_name[i]);
        }
     printf(
     "\n"
     "instruction 0:       0x%02x\n"
     "instruction 1:       0x%02x\n"
     "instruction 2:       0x%02x\n"
     "signature value:     0x%04x\n",
     boot->dos.instr0,
     boot->dos.instr1,
     boot->dos.instr2,
     leu16_to_host(boot->dos.signature));
    }

 else if (diskx == dos_atari)
    {
     printf(
     "Atari ST Boot disk sector information:\n"
     "OEM name:            ");
     for (i = 0; i < 6; i++)
        {
         if (boot->atari.oem_name[i] >= ' ')
            putchar(boot->atari.oem_name[i]);
        }
     printf(
     "\n"
     "instruction 0:       0x%02x\n"
     "instruction 1:       0x%02x\n"
     "serial number:       %d\n"
     "checksum:            0x%04x\n",     
     boot->atari.instr0,
     boot->atari.instr1,
     (boot->atari.serial[0] << 16) +
     (boot->atari.serial[1] << 8) +
     boot->atari.serial[2],
     beu16_to_host(boot->atari.checksum));
    }

 else if (diskx == dos_msx)
    {
     printf(
     "MSX-DOS Boot disk sector information:\n"
     "OEM name:            ");
     for (i = 0; i < 8; i++)
        {
         if (boot->msx.oem_name[i] >= ' ')
            putchar(boot->msx.oem_name[i]);
        }
     printf(
     "\n"
     "instruction 0:       0x%02x\n"
     "instruction 1:       0x%02x\n"
     "instruction 2:       0x%02x\n"
     "MSX-DOS 1 CE point:  0x%04x\n",
     boot->msx.instr0,
     boot->msx.instr1,
     boot->msx.instr2,
     leu16_to_host(boot->msx.msxdos1_cep));
     printf(
     "MSX-DOS 2 Vol sign:  ");
     for (i = 0; i < 6; i++)
        {
         if (boot->msx.msxdos2_volsign[i] >= ' ')
            putchar(boot->msx.msxdos2_volsign[i]);
        }
     printf(
     "\n"
     "MSX-DOS 2 Undelete:  0x%02x\n"
     "MSX-DOS 2 serial:    0x%08x\n"
     "reserved:            0x%02x\n"
     "MSX-DOS 2 CE point:  0x%04x\n"
     "signature value:     0x%04x\n",
     boot->msx.msxdos2_undel,
     leu32_to_host(boot->msx.msxdos2_serial),
     boot->msx.reserved,
     leu16_to_host(boot->msx.msxdos2_cep),
     leu16_to_host(boot->msx.signature));
    }

 printf(
 "sector size:         %d\n"
 "sectors per cluster: %d\n" 
 "reserved sectors:    %d\n" 
 "number of FATs:      %d\n"
 "root dir entries:    %d\n"
 "total sectors:       %d\n"
 "media descriptor:    0x%02x\n"
 "sectors per FAT:     %d\n"
 "sectors per track:   %d\n"
 "sides (heads):       %d\n"
 "hidden sectors:      %d\n",
 leu16_to_host(boot->bpb.bytes_sect),
 boot->bpb.sect_clust,
 leu16_to_host(boot->bpb.res_sect),
 boot->bpb.fats,
 leu16_to_host(boot->bpb.root_dir_entries),
 leu16_to_host(boot->bpb.total_sectors),
 boot->bpb.media_desc,
 leu16_to_host(boot->bpb.sect_per_fat),
 leu16_to_host(boot->bpb.sect_track),
 leu16_to_host(boot->bpb.number_sides),
 leu16_to_host(boot->bpb.number_hidden_sectors)
 );
}

//==============================================================================
// Detect various DOS disk/image formats.
//
// Read in the first sector. This will normally be a 512 byte sector but may
// use a different sector size so we need to probe.
//
// Atari ST disks don't use the signature bytes at 510 and 511 (0x1fe) so
// this is used to determine if disk is DOS or Atari.
//
// TODO:
// According to reference notes if sectors = 8 then most of the reserved
// sector values are not present and requires working out the disk
// structure using the media descriptor byte in the FAT.
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
int dos_detect_format (void)
{
 uint8_t buffer[1024];
 uint8_t fat[1024];

 int signature;
 int diskx;
 int secsize;
 int fm_s;
 int fm_f;
 int dr;
 int fat_md = 0;
 
 dos_bootstrap_u *boot = (dos_bootstrap_u *)buffer;
 
 DSK_FORMAT sector_id;

 // data rates arranged in order that works when a 360k in 1.2Mb drive it
 // should match up as RATE_DD when tested (don't have RATE_SD first!)
 dsk_rate_t datarates[4] = {RATE_DD, RATE_HD, RATE_SD, RATE_ED}; 

 dsk_err_t dsk_err = DSK_ERR_OK;

 // start with some generic DOS values, do not call format_set() after
 // datarate has been detected
 format_set("dos", 0);
 
 if (disk.verbose)
    printf("\n");

 // set retries for the input drive
 dsk_set_retry(idrive, 30);
 
 // if a raw image type is being used then the format_set() values will
 // determine the FM and datarate values unless options to override are
 // specified on the command line
 if (strcasecmp("raw", disk.itype) != 0)
    {
     if (dg_opts.ifm == -1)
        {
         fm_s = 0;
         fm_f = 1;
        }
     else
        {
         fm_s = xdg.dg_ifm;
         fm_f = xdg.dg_ifm;
        }

     // check MFM/FM modes and data rates
     for (dg.dg_fm = fm_s; dg.dg_fm <= fm_f; dg.dg_fm++)
        {
         // input datarate set with an option?
         if (dg_opts.idatarate == -1)
            {
             // no, then check all input datarates
             for (dr = 0; dr < 4; dr++)
                {
                 dg.dg_datarate = datarates[dr];
#if 0
                 printf("dr=%d drive type=%c dg.dg_datarate=%d dg.dg_fm=%d\n",
                 dr, disk.idrive_type, dg.dg_datarate, dg.dg_fm);
#endif
                 dsk_err = dsk_psecid(idrive, &dg, 0, 0, &sector_id);

                 if (! dsk_err)
                    break;
                }
            }
         else
            {
             // check a single datarate
             dg.dg_datarate = xdg.dg_idatarate;
             dsk_err = dsk_psecid(idrive, &dg, 0, 0, &sector_id);
            }

         if (! dsk_err)
            break;
        }
    }
#if 0
 else
    printf("drive type=%d dg.dg_datarate=%d disk.itype=%s\n\n",
    disk.idrive_type, dg.dg_datarate, disk.itype);
#endif

 // if was not able to detect MFM/FM or data rate exit with an error
 if (dsk_err != DSK_ERR_OK)
    {
     printf(
     "Could not sense media to determine recording mode or datarate.\n");
     detect_failed();
     return -1;
    }
    
 // read in the boot sector
 if (read_physical_sector(0, 0, 0, 0, 1, buffer) != DSK_ERR_OK)
    {
     printf("Could not read boot sector: Cylinder: %3d  Head: %2d"
     "  Result: %s\n", 0, 0, dsk_strerror(dsk_err));
     detect_failed();
     return -1;
    }

 memcpy(msxdos2_vs, boot->msx.msxdos2_volsign, 6);
 msxdos2_vs[6] = 0;

 // check if disk is DOS or other (Atari ST disk don't have this)
 signature = leu16_to_host(boot->dos.signature);
 diskx = (signature == 0xaa55 || signature == 0x0000) * dos_ms;
 
 // set Atari ST if not a PC/MS DOS disk or forced
 if (diskx != dos_ms || disk.detect == det_atari)
    {
     diskx = dos_atari;
     strcpy(xdg.dg_format_name, "atari"); // format name
    }

 // search the boot sector for 'MSXDOS' and 'VOL_ID' and if either found
 // consider it to be a MSXDOS 1 or 2 disk, or use --detect=msx option to
 // force
 if (memstr(buffer, sizeof(buffer), "MSXDOS") ||
 strcasecmp(msxdos2_vs, "VOL_ID") == 0 ||
 disk.detect == det_msxdos)
    {
     diskx = dos_msx;
     strcpy(xdg.dg_format_name, "msxdos"); // format name
    }

 // get low level sectors information from the disk BPB
 secsize = leu16_to_host(boot->bpb.bytes_sect);

 // if a DOS disk then we need to test some of the values in 3-29 to see if
 // they are absent, if so we assume an OLD 8 sect/track 0xFE or 0xFF media
 if (diskx == dos_ms && secsize != 512)
    {
     // read in the 1st FAT sector
     if (read_physical_sector(0, 0, 0, 0, 2, fat) != DSK_ERR_OK)
        {
         printf("Can't read the FAT sector: Cylinder: %3d  Head: %2d"
         "  Result: %s\n", 0, 0, dsk_strerror(dsk_err));
         detect_failed();
         return -1;
        }

     if (*fat == 0xfe || *fat == 0xff)
        {
         dg.dg_secsize = 512;
         dg.dg_sectors = 8;
         dg.dg_heads = (*fat == 0xfe)? 1:2;
         dg.dg_cylinders = (*fat == 0xfe)? 320:640 / 8 / dg.dg_heads;
         disk.mediadesc = *fat;
         fat_md = *fat;
        }
     else
        {
         printf("Expected an 8 sector/track media descriptor of 0xfe or 0xff\n"
         "but found 0x%02x instead!\n", *fat);
         detect_failed();
         return -1;
        }
    }
 else
    {
     // check the media descriptor byte is 0xF?
     if ((boot->bpb.media_desc & 0xf0) != 0xf0)
        {
         printf("Media descriptor byte not recognised (0x%02x)\n",
         boot->bpb.media_desc);
         detect_failed();
         return -1;
        }

     dg.dg_secsize = secsize;
     dg.dg_sectors = leu16_to_host(boot->bpb.sect_track);
     dg.dg_heads = leu16_to_host(boot->bpb.number_sides);
     dg.dg_cylinders = leu16_to_host(boot->bpb.total_sectors)
     / dg.dg_sectors / dg.dg_heads;
     disk.mediadesc = boot->bpb.media_desc;
    }

 // check for sensible sides (heads) and sector size values
 if (
    (dg.dg_heads < 1 || dg.dg_heads > 2) ||
    (dg.dg_secsize != 128 && dg.dg_secsize != 256 &&
     dg.dg_secsize != 512 && dg.dg_secsize != 1024))
    {
     printf(
     "Bad number of heads or sector size found, probably not a DOS disk.\n");
     detect_failed();
     return -1;
    }

 // fill in the input datarate and MFM/FM values to be used elsewhere
 xdg.dg_idatarate = dg.dg_datarate;
 xdg.dg_ifm = dg.dg_fm; 
 
 // set the output datarate value to match the input value if it was not
 // specified
 if (dg_opts.odatarate == -1)
    xdg.dg_odatarate = xdg.dg_idatarate;

 // set the output MFM/FM value to match the input value if it was not
 // specified
 if (dg_opts.ofm == -1)
    xdg.dg_ofm = xdg.dg_ifm;

 // set the disk description
 sprintf(xdg.dg_format_desc, "%s %s %s %s %dx%dx%d media:%02x",
 dos_names[diskx - 1],
 (dg.dg_heads > 1)? "ds":"ss",
 datarates_str[xdg.dg_odatarate],
 xdg.dg_ofm? "fm":"mfm",
 dg.dg_cylinders, dg.dg_sectors, dg.dg_secsize, disk.mediadesc);
 
 // if an early 8 sector/trask DOS disk 
 if (fat_md)
    printf("The media appears to be a %s 8 sector/track disk, the FAT media\n"
    "descriptor is 0x%02x. BPB bytes 3-29 are not used.\n\n",
    (fat_md == 0xfe)? "SS":"DS", fat_md);

 // report the DOS/Atari ST/MSX-DOS disk information 
 if (disk.verbose)
    detect_information(boot, diskx);

 return 0;
}
