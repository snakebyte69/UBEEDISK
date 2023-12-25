//******************************************************************************
//*                                 uBeeDisk                                   *
//*                                                                            *
//*     A tool for converting disks/images from one to another with auto       *
//*                   detection options for Microbee disks.                    *
//*                                                                            *
//*                              detect module                                 *
//*                                                                            *
//*                       Copyright (C) 2008-2017 uBee                         *
//******************************************************************************
//
// Probe for known Microbee disk formats.
//
//==============================================================================
// ChangeLog (most recent entries are at top)
//==============================================================================
// v4.0.0 - 25 January 2017, uBee
// - Added GAP set and erase values to format_set_geometry().
// - Added a ATTR_CACHER_OFF and ATTR_CACHEW_OFF option to the
//   disk_format->attributes to disable read/write caching in devices that
//   support it.
// - Added 'dos' detection call functions.
// - Changed "char *buffer" to "uint8_t *buffer" in read_physical_sector().
// - Changes to get_drive_ready() only to use if host floppy drive.
//
// v3.0.0 - 9 January 2015, uBee
// - Added new sideoffs and attribute member to format_set_geometry().
// - Added various support to various_disk_format[] table, etc.
// - Changes to format_set() and added new format_search_id() function.
// - Added parameter to enable reporting information in format_set().
// - Changes to format_list_formats() to double width of left column.
// - More informative format_list_types(), code from LibDsk (utilopts.c)
//
// v2.2.0 - 12 May 2012, uBee
// - Removed get_drive_type() call as now only called from ubeedisk.c
// - Removed all *no_dsk_*[] code and used the sup_t input_sup structure
//   instead where required.
//
// v2.1.0 - 7 October 2011, uBee
// - Added c5h0s0_525 format. (not tested or part of auto detection)
// - All built-in formats have beeen moved to other files and the coding in
//   this file changed to accomodate.
//
// v2.0.0 - 30 September 2011, uBee
// - Added Microbee Gamma, Applix 1616, ibm3740 and default formats.
// - Changes made to the 'user' format.
// - Changed some formats where the rwgap and fmtgap values are not known to
//   0, this will prevent writing to floppy, remote, etc.
// - Removed disktype member from disk_format_t structure.
// - Added skew_val and skew_ofs members to disk_format_t structure.
//
// v1.6.0 - 29 July 2010, uBee
// - Added Microbee HDD formats 'teac - Microbee TEAC HDD 10MB 306
//   cylinders, 4 heads, 17 sect/track' and 'mscribe - Microbee Miniscribe
//   HDD 10MB 612 cylinders, 2 heads, 17 sect/track' to disk_format[]
//   structure.
//
// v1.4.0 - 19 March 2010, uBee
// - Fixed problem in read_sector_ids() function by making use of the
//   DSK_ERR_NOTIMPL return value and report the problem.  The user is then
//   advised to use the '--format' option.
// - Added verbose level for some console output. See ChangeLog in ubeedisk.c
//
// v1.2.0 - 3 February 2010, uBee
// - Removed dsk_set_retry() function where it was setting retries back to 1.
//
// v1.0.0 - 11 December 2008, uBee
// - Initial file created.

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
#include "ubeedisk.h"
#include "functions.h"

#include "microbee.h"
#include "applix.h"
#include "dos.h"
#include "fm.h"
#include "various.h"

extern sup_t input_sup;
extern sup_t output_sup;

extern disk_format_t microbee_disk_format[];
extern disk_format_t applix_disk_format[];
extern disk_format_t dos_disk_format[];
extern disk_format_t fm_disk_format[];
extern disk_format_t various_disk_format[];

extern DSK_PDRIVER idrive;
extern DSK_GEOMETRY dg;
extern disk_t disk;
extern xdg_t xdg;

//==============================================================================
// structures and variables.
//==============================================================================
char *detect_disk_args[] =
{
 "off",
 "mbee",
 "mbee3",
 "mbee5",
 "applix",
 "dos",
 "atari",
 "msx",
 "fm",
 "various",
 ""
};

static disk_format_t *disk_formats[] =
{
 microbee_disk_format,
 applix_disk_format,
 dos_disk_format,
 fm_disk_format,
 various_disk_format,
 NULL
};

//==============================================================================
// get drive ready.
//
//   pass: void
// return: void
//==============================================================================
static void get_drive_ready (void)
{    
 dsk_err_t dsk_err = DSK_ERR_OK;
 DSK_FORMAT sector_id;

 if (! disk.first_read)
    return;

 disk.first_read = 0;
 
 // if not a host PC floppy then return
#ifdef WIN32
 if (strcmp("ntwdm", disk.itype) != 0)
#else
 if (strcmp("floppy", disk.itype) != 0)
#endif
    return;

 // some drives need this or the disk head won't get positioned and we end
 // up with no data error! (must be before close_reopen_pc_floppy_input_drive())
 dsk_err = dsk_psecid(idrive, &dg, 0, 0, &sector_id);

 // must close/reopen the input drive and restore settings if PC floppy
 close_reopen_pc_floppy_input_drive();

 // some drives error out as it seems there is not enough delay
 // after the head is homed so for the first read error we will do
 // an unreported retry. (i.e Tandon TM848-02 8") 
 if (dsk_err == DSK_ERR_OK)
    return;

 // don't use close_reopen_pc_floppy_input_drive() here! instead just
 // use a delay or the head will try to home constantly.
 sleep_ms(3000);
}

//==============================================================================
// Find track information.
//
// This process uses a function call to return all the sectors found on a
// track.  The 1st sector number found may not start with 1 and some early
// sectors may be missed.
//
//   pass: int cly                      cylinder number
//         int head                     physical side of disk
//         int secsize                  size of sectors to be checked for
//         int *sizeok                  sector sizes ok flag
//         int *size                    size of last sector read
//         dsk_psect_t *count           count of sector headers read
//         int *t2hx_sec_f              first sector (by value)
//         int *t2hx_sec_l              last sector (by value)
// return: dsk_err_t                    0 if successful
//==============================================================================
dsk_err_t read_sector_ids (int cyl, int head, int secsize, int *sizeok,
                           int *size, dsk_psect_t *count,
                           int *txhx_sec_f, int *txhx_sec_l)
{
 dsk_err_t dsk_err = DSK_ERR_OK;

 DSK_FORMAT *result;
 DSK_FORMAT sector_id;
 
 int i;

 *count = -1;
 *size = -1;
 *sizeok = 1;
 *txhx_sec_f = 1000;
 *txhx_sec_l = 0;

 // set data rate for input
 dg.dg_datarate = xdg.dg_idatarate;

 // set retry count to a reasonable value
 dsk_set_retry(idrive, 3);  // may need to increase this

 get_drive_ready();

 // avoid dsk_ptrackids() for types that do not support the function
 if (! input_sup.ptrackids)
    dsk_err = DSK_ERR_NOTIMPL;
 else
    // read cylinder n, side n of the disk
    dsk_err = dsk_ptrackids(idrive, &dg, cyl, head, count, &result);

 if (dsk_err == DSK_ERR_NOTIMPL)
    {
     printf("The input driver does not support the dsk_ptrackids()"
            " function.\n"
            "Use the '--format' option to specify a disk format.\n");
     return dsk_err;
    }

 if ((dsk_err == DSK_ERR_OK) && (*count > 1))
    {
     // find the smallest and highest sector numbers
     for (i = 0; i < *count; i++)
        {
         sector_id = result[i]; // sector position
         if (sector_id.fmt_sector < *txhx_sec_f)
            *txhx_sec_f = sector_id.fmt_sector;

         if (sector_id.fmt_sector > *txhx_sec_l)
            *txhx_sec_l = sector_id.fmt_sector;

         *size = sector_id.fmt_secsize; // save size

         if (*size != secsize)
            *sizeok = 0;
        }    
    }

 if (dsk_err == DSK_ERR_OK)
    dsk_free(result); 

 return dsk_err;
} 

//==============================================================================
// Read one physical sector.
//
// Intended to be called from the detection modules if needed after a format
// has been set.
//
//   pass: dsk_pcyl_t cyl               physical drive cylinder number
//         dsk_pcyl_t xcyl              ID cylinder value
//         dsk_phead_t head             physical drive side
//         dsk_phead_t xhead            ID side value
//         int psect                    physical sector number
//         uint8_t *buffer              destination pointer
// return: dsk_err_t                    DSK_ERR_OK if successful
//==============================================================================
dsk_err_t read_physical_sector (dsk_pcyl_t cyl, dsk_pcyl_t xcyl,
                                dsk_phead_t head, dsk_phead_t xhead,
                                int psect, uint8_t *buffer)
{
 dsk_err_t dsk_err = DSK_ERR_OK;

 get_drive_ready();

 dsk_err = dsk_xread(idrive, &dg, buffer, cyl, head, xcyl, xhead, psect,
 dg.dg_secsize, NULL);
 
 if (dsk_err == DSK_ERR_NOTIMPL)
    dsk_err = dsk_pread(idrive, &dg, buffer, cyl, head, psect);

 return dsk_err;
}

//==============================================================================
// Set a disk format geometry.
//
// The *disk_format is a pointer to one of the built in disk formats table
// structures.
//
// The 'xdg' structure are extensions to the LibDsk 'dg' format but are only
// used by ubeedisk.
//
// The 'xdg.dg_idatarate', 'xdg.dg_odatarate', xdg.dg_ifm' and 'xdg.dg_ofm'
// are set using the format's data rate and recording modes, those values
// may be overriden later for input and output using --datarateip,
// --datarateop, --fmip, --fmop.  The --datarate and --fm options will set
// input and ouput rates and FM modes to the same value.
//
// The 'dg.dg_datarate' and 'dg.dg_fm' values are set here but needs to be
// set elsewhere with values from 'xdg.dg_idatarate', 'xdg.dg_odatarate'
// 'xdg.dg_ifm', 'xdg.dg_ofm' values depending on a read/write operation.
//
//   pass: disk_format_t *disk_format
// return: int                          0 if successful else -1
//==============================================================================
int format_set_geometry (disk_format_t *disk_format)
{
 dg.dg_sidedness = disk_format->sidedness;    // how to handle multisided discs

 dg.dg_cylinders = disk_format->cylinders;    // number of cylinders
 dg.dg_heads = disk_format->heads;            // number of heads
 dg.dg_sectors = disk_format->sectors;        // sectors per track
 dg.dg_secbase = disk_format->secbase;        // first physical sector number
 dg.dg_secsize = disk_format->secsize;        // sector size in bytes
 dg.dg_datarate = disk_format->datarate;      // data rate
 dg.dg_rwgap = disk_format->rwgap;            // read/write gap length
 dg.dg_fmtgap = disk_format->fmtgap;          // format gap length
 dg.dg_fm = disk_format->fm;                  // not FM mode
 dg.dg_nomulti = disk_format->nomulti;        // disable multitrack
 dg.dg_noskip = disk_format->noskip;          // don't skip deleted data

 xdg.dg_side1as0 = disk_format->side1as0;     // side1as0 issue
 xdg.dg_secbase1s = disk_format->secbase;     // 1st sector base number
 xdg.dg_secbase2c = disk_format->secbase2_c;  // 2nd sector base cyl change
 xdg.dg_secbase2s = disk_format->secbase2_s;  // 2nd sector base first sector
 xdg.dg_idatarate = disk_format->datarate;    // input data rate
 xdg.dg_odatarate = disk_format->datarate;    // output data rate
 xdg.dg_ifm = disk_format->fm;                // input FM or MFM recording mode
 xdg.dg_ofm = disk_format->fm;                // output FM or MFM recording mode
 xdg.dg_odatarate = disk_format->datarate;    // output data rate
 xdg.dg_sideoffs = disk_format->sideoffs;     // side offset value

 // skew values used for speeding up read/writes
 if (disk_format->skew_val == -1)             // sector skew value
    xdg.dg_skew_val = SKEW_VAL_DEF;
 else
    xdg.dg_skew_val = disk_format->skew_val;

 if (disk_format->skew_ofs == -1)             // offset to apply to sector base
    xdg.dg_skew_ofs = SKEW_OFS_DEF;
 else
    xdg.dg_skew_ofs = disk_format->skew_ofs;
    
 xdg.dg_special = disk_format->special;       // special disk formats
 xdg.dg_attributes = disk_format->attributes; // attribute flags

 strcpy(xdg.dg_format_name, disk_format->name); // format name
 strcpy(xdg.dg_format_desc, disk_format->desc); // format description

 // other values
 disk.mediadesc = -1;                         // set by DOS detect module

 // call-back functions
 xdg.ssr_cb = disk_format->ssr_cb;
 xdg.rsi_cb = disk_format->rsi_cb;
 xdg.ssd_cb = disk_format->ssd_cb;
 xdg.cif_cb = disk_format->cif_cb;
 xdg.pss_cb = disk_format->pss_cb;

 // physical skew table data pointers for sides, this allows different
 // physical skewing for each side but if the skewing is more involved then
 // need to have a custom 'pss_cb' entry in the module concerned.
 xdg.dg_pskew0 = disk_format->pskew0;
 xdg.dg_pskew1 = disk_format->pskew1;

 // if a GAPs string is defined
 if (*disk_format->gap)
    if (get_gap_colon_arguments(disk_format->gap, disk.gap_set,
    4, 0x3fff) == -1)
       return -1;

 // formats starting with "erase-" are special track erase formats
 if (strcasestr(disk_format->name, "erase-") == disk_format->name)
    disk.erase = 0;   // value to be used for erasing

 // if the format is requesting for write caching to be disabled
 if (disk_format->attributes & ATTR_CACHEW_OFF)
    disk.cachew = 0;

 // if the format is requesting for read caching to be disabled
 if (disk_format->attributes & ATTR_CACHER_OFF)
    disk.cacher = 0;
    
 return override_values();  // set double stepping, data rates, etc.
}

//==============================================================================
// List all disk formats.
//
//   pass: void
// return: void
//==============================================================================
void format_list_formats (void)
{
 disk_format_t *disk_format;
 int fmt;
 dsk_cchar_t fdesc;
 dsk_cchar_t fname;
 int f;
 int i = 0;

 printf("Formats built into "APPNAME":\n");

 // display the names in all modules
 while ((disk_format = disk_formats[i++]))
    {
     f = 0;
     while (disk_format[f].name[0] != 0)
        {
         printf("%-20.20s : %s\n", disk_format[f].name, disk_format[f].desc);
         f++;
        }
    }

 printf("\nFormats built into LibDsk (and libdskrc):\n");
 fmt = FMT_180K;
 while (dg_stdformat(NULL, fmt++, &fname, &fdesc) == DSK_ERR_OK)
    printf("%-20.20s : %s\n", fname, fdesc);
}

//==============================================================================
// List all disk types.
//
//   pass: void
// return: void
//==============================================================================
void format_list_types (void)
{
 int n = 0;
 char *type;
 char *desc;
 
 printf("Disk image types supported:\n\n");
 while (!dsk_type_enum(n, &type)) 
    {
     dsk_typedesc_enum(n++, &desc);
     printf("   %-10.10s : %s\n", type, desc);
    }
}

//==============================================================================
// Detect disk in drive.
//
// Disk detection for fm formats can be added here and the --detect option
// can then be used to select the one required.
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
int format_detect_disk (void)
{
 int res;

 if (disk.detect && disk.verbose)
    printf("\nDetecting '%s' disk format...\n", detect_disk_args[disk.detect]);

 switch (disk.detect)
    {
     case det_mbee : // detect Microbee formats
     case det_mbee3 :
     case det_mbee5 :
        res = microbee_detect_format();
        break;
     case det_applix : // detect Applix 1616/OS formats
        res = applix_detect_format();
        break;
     case det_dos : // detect various DOS/Atari ST formats
     case det_atari :
     case det_msxdos :
        res = dos_detect_format();
        break;        
     case det_fm : // detect FM disk formats
        res = fm_detect_format();
        break;        
     case det_various : // detect various disk formats
        res = various_detect_format();
        break;        
     default :
        printf(APPNAME": use the --detect option to select a type.\n");
        return -1;
    }
 return res;
}

//==============================================================================
// Search disk format tables for format ID string match.
//
// This checks for internal formats defined in the format modules,  if the
// format is not found then -1 is returned.
//
// It checks all the tables in disk format for a match and uses the first
// one found.  This means that all format_id table values must have unique
// values.
//
//   pass: char *format_id              name of the detection type
//         int *fmt                     format table index number of match
// return: disk_format_t *disk_format   pointer to a disk format table if match
//==============================================================================
static disk_format_t *format_search_id (char *format_id, int *fmt)
{
 disk_format_t *disk_format;
 int i = 0;

 // look through each disk format table defined in disk_formats[] until the
 // first occurence of the format_id string value is found
 while ((disk_format = disk_formats[i++]))
    {
     *fmt = 0;
     for (;;)
        {
         if (disk_format[*fmt].name[0] == 0)
            {
             *fmt = -1;
             break;
            } 
         if (strcasecmp(format_id, disk_format[*fmt].name) == 0)
            break;
         (*fmt)++;
        }
    
     // return the disk format table pointer (*fmt is the index)
     if (*fmt != -1)
        return disk_format;
    }

 return NULL;
}

//==============================================================================
// Set disk format to use.
//
// This checks for internal formats defined in the format modules,  if the
// format is not found then -1 is returned and the calling process may then
// check for formats internal to LibDsk or defined in the libdskrc file.
//
//   pass: char *format_id              name of the detection type
//         int report                   allow reporting the format just set
// return: int                          fmt number if successful else -1
//==============================================================================
int format_set (char *format_id, int report)
{
 disk_format_t *disk_format;
 int fmt;

 // search disk format tables for format ID string match
 disk_format = format_search_id(format_id, &fmt);

 if (disk_format)
    {
     if (format_set_geometry(&disk_format[fmt]) == -1)
        return -1;

     if (disk.verbose && report)
        printf("\nInternal format selected:\n%s: %s\n", disk_format[fmt].name,
        disk_format[fmt].desc);
         
     return fmt;
    }

 return -1;
}

//==============================================================================
// Set detect disk type.
//
//   pass: char *format_id              name of the detection type
// return: int                          detection # if successful else -1
//==============================================================================
int format_detect_select (char *format_id)
{
 int x;
 
 x = string_search(detect_disk_args, format_id);

 if (x == -1)
    return -1;
    
 disk.detect = x;
 return disk.detect;   
}
