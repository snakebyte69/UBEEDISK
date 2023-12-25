//******************************************************************************
//*                                 uBeeDisk                                   *
//*                                                                            *
//*      A tool for converting disks/images from one to another with auto      *
//*                  detection options for Microbee disks.                     *
//*                                                                            *
//*                       Copyright (C) 2008-2017 uBee                         *
//******************************************************************************
//
// This program permits copying and creating disks and images from one to
// another.  The program was created to target Microbee disk formats and to
// provide additional data recovery methods and creating info files.
//
// The program makes use of the 'LibDsk' library and can be found here:
// http://www.seasip.demon.co.uk/Unix/LibDsk/
//
// Parts of this program are based on the tools supplied with LibDsk.
//
//==============================================================================
// ChangeLog (most recent entries are at top)
//==============================================================================
// v4.0.0 - 26 January 2017, uBee
// - Added GAPSET and SYNCSET values to override_values().
// - Added disk.cacher and disk.cachew values to override_values().
// - Added disk.erase values to override_values().
// - Added new output_filename_substitution() functionality.
// - Changed datarate scan order in disk_scan() and disk_speed().
// - Added report_dg() to report geometry values (when verbose > 1).
// - Added a warning message if config file is an old version.
// - Added 'IMG' images to set_xtype_xfile() ext_type[] table (raw).
// - Changes to enter_disk_description() to allow quitting (no disk copy).
// - New format table structures for IBM 3740 and IBM SYSTEM 34.
// - Changed fdc_gap_calc() name to fdc_buffer_format().
// - Recoded fdc_buffer_format() and format_track_size().
// - Removed buffering information in create_info_file().
// - All jobs now return an error status that may be used in scripts.
// - Changes in override_values() to increase default 'remote' type
//   disk.retries_l1 value from 1 to 3.
// - Changes in various locations to replace dg_opts.datarate with
//   dg_opts.idatarate and dg_opts.odatarate values and dg_opts.fm with
//   dg_opts.ifm and dg_opts.ofm.
// - Removed all 'disk.error' variable usage.
// - Changes to disk_format() so that the format type is shown before the
//   WARNING message prompt for action.
// - Various code cleanup and fixes.
//
// v3.1.0 - 14 November 2016, uBee
// - Added 'disk.nofill' option to copy_one_disk() function. (see --nofill).
// - Appears that LibDsk DSK and RAW drivers do not support the --iside and
//   --oside options and only works for floppy access?  Provide a work
//   around if setting --iside or --oside and not supported by driver.
// - Added 'IMD' images to set_xtype_xfile() ext_type[] table.
// - Added a hint to one of the fdc_gap_calc() error messages.
//
// v3.0.0 - 16 January 2015, uBee
// - Added optional dynamic calculation of GAP bytes used for formatting
//   floppy disks.  Used the format_track_size() and fdc_gap_calc()
//   functions from closed Floppyio firmware source with minor changes. 
//   Static GAP values may still be defined and will be used instead.  To
//   use dynamic GAP values define GAP values with 0.
// - Added new 'info' file entries for the input/output drive types used.
// - Added write error reporting for the first 10 errors in
//   write_buffered_track() and removed the (verbose > 1) reporting from
//   copy_one_disk().
// - Changes and fixes to format_track() so that only floppy types use
//   physical skewing where a format dictates.  Skewing tables are now
//   located in the respective format modules with new table entries added.
// - Changes to set_start_finish() to handle cylinders and logical sectors.
// - Replaced specific disk format handling code with xdg.*_cb() call-back
//   functions which are defined in the disk support modules if needed.
// - Added xdg.dg_sideoffs in various places to support different side
//   values (i.e.  128 and 129) where these are used.
// - Added checking to open_input_drive() and create_output_drive() for the
//   new attribute flags to determine if the disk format has a variable
//   track size and a suitable type is specified to for it.
// - Changes and fixes to disk_scan(). Now only checks different data rates,
//   etc if last scan was error which speeds up the process.  A 400/800 kb
//   DD disk in 1.2 Mb drive fixed to report as 300 and not 250 kHz and for
//   FM disks to report at the correct data rate.
// - Changes to disk.select to remove the auto 'mbee' setting of it, the
//   user must now specify it on the command line if detection is required.
// - Changes to pause_menu() and disk_clean() to take the last used cylinder
//   value so that the head position can be restored before a clean process
//   completes.
// - Changes to overwrite_permission() when prompting for overwrite action.
// - Added 'serial:' remote file detection to set_xtype_xfile().
// - Added s80_cpm1 and compak format structure to format_track() for
//   Super-80 CP/M and Amust Compak.
// - Added special Super-80 CP/M and Compak flag/code to set_special_disk().
// - Fixed read_sector_id() to return the correct xhead value when
//   disk.side1as0 is set and the format does not support the dsk_psecid()
//   function or a read error occurs.
// - Fixed segmentation error in create_info_file() that occurs there is a
//   disk error and using batch mode. (needed extra errorf = NULL).
// - Fixed memset(xsup, 0, sizeof(xsup)) to use 'sup_t' for size and is now
//   memset(xsup, 0, sizeof(sup_t)) in interface_support()
// - Changes made to enable/disable the Floppy disk work around handler.
// - Pruning of overuse of curly brackets and various source changes.
//
// v2.3.1 - 11 January 2014, uBee
// - Fixed format_track() where Microbee DS80 DSK images were not using the
//   correct sector numbers when format_track_range() was being used for
//   partial formatting of a disk (i.e.  when using the --start and --finish
//   options).  DS80 was incorrectly using sectors 1-10 instead of 21-30 for
//   data tracks.  Any formats that make use of xdg.dg_secbase2c to specify
//   data cylinders start point will now use the sector base value held in
//   xdg.dg_secbase2s.
//
// v2.3.0 - 4 June 2013, uBee
// - Changes to reset_input_drive_and_settings() to exit if not a PC Floppy
//   disk as input type.  This function was causing Remote time-out errors
//   and the code should not be necessary.  Added error reporting and
//   exit(EXIT_FAILURE).
// - Changes to close_files() to report any dsk_close() errors.
// - Removed unused variables xcyl and xhead from disk_speed().
//
// v2.2.0 - 14 May 2012, uBee
// - Added xdrive parameter to get_drive_type() calls.
// - Removed all the *no_dsk_*[] code and used a new sup_t input_sup
//   structure to replace it.  Added interface_support() function for this.
// - Removed from override_values() RWGAP and FMTGAP check code as will fail
//   first time through and was decided it's not needed anyway?
// - Complete sector ID header value control using extra forceside values.
// - Changes to create_info_file() to report the extra forceside values.
// - Fixed copy_one_disk() format track to only use the disk format's
//   'side1as0' value when the read source type does not support a fully
//   implemented psecid function.
// - Changes made to disk_clean() process to only run if input is a floppy
//   drive.
// - LibDsk call back reporting requires setting verbose level 2 or higher.
// - Added dsk_set_option(idrive, "AUTORATE", 0) for drivers that support
//   this option and support auto rate detection being disabled.
// - Added overwrite_permission() to decide what to do if destination file
//   already exists.
// - Moved pause menu into own function pause_menu() and is also now called
//   from disk_scan(), disk_format() and disk_speed().
// - Disable exiting using ctrl-C, ctrl-\ and ctrl-Z under Unix systems and
//   ctrl-C under windows as can leave floppy in an unstable state.
// - get_drive_type() now only called from open_input_drive() and
//   create_output_drive().
//
// v2.1.0 - 7 October 2011, uBee
// - Added Abort option to the Pause menu.
// - Added Applix disk skewing to format_track() function.
// - Changes to make reading of all sectors use sector skewing regardless of
//   the buffering mode.  The buffering mode (quiet) is only different in
//   that it does not do any console output and minimal processing between
//   sector reads which might cause sectors to be missed.
// - Changes to read_sector_retry() to report logical and physical sectors.
// - Changes to use logical sector numbers before skew translations.
// - Changes to info_file_entry() to handle skewed sector insertions and to
//   re-instate unread sector flags.
// - Fixed recovery option if drive door is opened when reading a sector and
//   changed the action for the Retry method.
// - Fixed the FLAGS_C5H0S0 format which was broken in v2.0.0.
// - Fixed copy_one_disk() to write out complete info file when copy process
//   is aborted.  This places an 'X' into each unread sector location within
//   the info file.
// - Fixed create_info_file() to report 'X' for unread sector locations.
// - Fixed set_start_finish() to allow override of track start/finish.
// - Changes made to format_set_geometry() parameter passing.
//
// v2.0.0 - 1 October 2011, uBee
// - Added 'speed' argument to --disk option. This may be used to test and
//   adjust the RPM setting of the spindle motor.
// - Added 'clean' argument to --disk option. This may be used to clean the
//   drive heads using a special cleaning disk.
// - Added track buffering functions read_buffered_track() and
//   write_buffered_track() which greatly speeds up the copy process.
// - Added configurable skewed reading of sectors in track read buffering.
// - Added key detection while copying is in progress, this allows the
//   operator to pause the process.  The operator may then clean the drive
//   heads and continue or switch between Unattended and Interactive modes.
// - Unattended mode is now disabled by default.
// - Changes to read_sector_retry() to allow error handling mode to be
//   changed from the Interactive menu.
// - Changes to read_sector_retry() when using the Interactive menu to
//   Ignore (Skip) all the remaining errors that are encountered for the
//   current disk.
// - Changes to read_sector_retry() prompts.
// - Fixed the disk.oside setting in override_values(), was using wrong
//   condition variable of disk.iside instead of disk.oside which meant the
//   output side was set wrong when disk.iside is not the default unless
//   disk.oside had also been set with an option.
// - disk_scan() on some disks procuded a non recoverable error which
//   prevented all tracks from being scanned successfully.  Added some code
//   to work around this.
// - disk_scan() was incorrectly making use of the --iside option. Changes
//   made so that --scanhead option can specify the start and end heads for
//   scanning.  The default head range is 0-1.
// - disk_scan() now reports LibDsk error when a track can't be scanned.
// - Added read_sector_x() function to replace code used in read_sector().
// - Changes to set_rate_step() to prevent double stepping on auto detect
//   for input/output drives if --idstep=off or --odstep=off are specified.
//   Also now only sets if disk is less than 70 tracks.
// - Changes to read_sector() as some drives error out as it seems there is
//   not enough delay after the head is homed so for the first read error we
//   will do an unreported retry.
// - Added code to override_values() to disallow writing to floppy, remote,
//   etc. if any GAP values are set to 0.
// - Added reset_input_drive_and_settings() function.
// - format_set_geometry(mb_ds84) is now format_set_geometry(xx_default).
// - Removed dg_disktype member from xdg_t structure.
// - Added 'special' member to dg_opts_t structure and added handling for
//   flag '60' disks.
// - Changes to format_track() to improve error reporting.
//
// v1.6.1 - 3 March 2011, uBee
// - Fixed reading (and writing?) for Honeysoft disks when using a disk
//   driver type that does not support 'dsk_psecid' (i.e 'remote' and
//   FDS.COM on the Microbee computer).  Access past the maximum of 5
//   sectors was occuring on 1024 byte sector tracks, this was fixed by
//   adding some code into read_sector_id().
//
// v1.5.0 - 7 July 2010, uBee
// - Removed dsk_set_retry(idrive, disk.retries_l1) from copy_one_disk() as
//   this was being overridden in read_sector_id().  The dsk_set_retry()
//   call in read_sector_id() is now used to set the retry value for
//   subsequent sector reads.
// - Added code to override_values() to set the disk.retries_l1 value if no
//   option has been specified.  This value is set to 1 when the input drive
//   type is 'remote' as it was taking too long with all the uneccesary
//   repeated retries on native hardware.  When using the LibDsk driver we
//   tend to see a lot of soft errors so the value needs to be higher.
// - Changed disk_scan() to now use dsk_set_retry(idrive, disk.retries_l1)
//   instead of dsk_set_retry(idrive, 5).  The appropriate value for
//   disk.retries_l1 when scanning is set in an earlier call to
//   override_values().
// - Changes made to fgets() function to use the result as some compilers
//   report warning: declared with attribute warn_unused_result.
//
// v1.4.0 - 18 March 2010, uBee
// - Fixed problems in read_sector_id() function and other areas by making
//   use of the DSK_ERR_NOTIMPL return value. This value when returned will
//   cause a more standard function call to be used instead. The 'raw' and
//   'remote' types now works when used as an input.  Others types may also
//   be working now if they didn't before.
// - Added verbose level for some console output. The default level is 1
//   which makes all console messages active like previous versions.  If set
//   to 0 using the --verbose option then only error messages, interaction
//   prompts and processes intended to convey information will be output.
// - Added more types to be excluded when creating 'info', and '.err' files. 
//   Only output types that are images will be able to create these files.
//
// v1.3.0 - 12 February 2010, uBee
// - Added data rate and encoding mode detection and reporting to the
//   disk_scan() process.
// - Changes to disk_scan() process so that scanning can be instructed what
//   side of the disk to read with the --iside option.  If set to default of
//   -1 then both sides are read.
// - Fixed a problem when using batch mode (--count=n) the 'auto_seeked_count'
//   variable was not being set back to 0 for the next copy operation.
// - Fixed disk_copy() to set detection based on disk.itype, not disk.otype.
// - Fixed disk info command (disk_geometry) and moved down.
// - Cleaned up some code in main().
//
// v1.2.0 - 5 February 2010, uBee
// - Added an unattended operation mode including additional logging
//   information when copying disks.
// - Changes concerning the reading of sectors that was causing many soft
//   retries to show up in the '.info' file and quite likely to have caused
//   problems on some disks because of a too low retry value at the LibDsk
//   level.  Default value of 'disk.retries' (now disk.retries_l2) is now 3
//   instead of 30 and at the LibDsk level (disk.retries_l1) is now 10
//   instead of 1 and can be changed using an option.
// - Disabled the compile of code to test the 'dsk_err_wrt' variable result
//   in copy_one_disk() function as this was failing when writing to disk
//   images depending whether an ignore error response occured earlier when
//   reading a disk.  The actual problem here is still to be established.
// - A new --force option is used to allow the '--disk=format' option to
//   format a disk without user confirmation.
// - Error diagnostics will now be logged to a '.err' file by default
//   except when the --append-error option is specified which will cause
//   the error log to be appended to the 'info' file as before.
//
// v1.1.0 - 10 March 2009, uBee
// - Transient errors are now processed just like any other disk errors. The
//   error count now also increments when one of these errors occurs.
// - Added error report logging, this is appened to the info file.
//
// v1.0.0 - 8 December 2008, uBee
// - Initial program created.

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
#include <time.h>

#include <libdsk.h>

#ifdef WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

#include "ubeedisk.h"
#include "format.h"
#include "options.h"
#include "getopt.h"
#include "functions.h"
#include "strverscmp.h"


//==============================================================================
// structures and variables
//==============================================================================
disk_t disk =
{
 .iautorate = 1,
 .oautorate = 1, 
 .cacher = -1,
 .cachew = -1, 
 .count = -1,
 .enter_desc = 1,
 .erase = -1,
 .fd_workaround1 = 1,
 .fd_workaround2 = 1, 
 .finish = -1,
 .gap_set = {-1, -1, -1, -1, -1, -1, -1, -1},
 .head_s = 0,
 .head_f = 1,
 .idstep = -1,
#ifdef WIN32
 .ifile = "A:",
#else
 .ifile = "/dev/fd0",
#endif
 .info_file = 1,
 .iside = -1,  // -1 is both sides
 .log = 1,
 .oside = -1,  // -1 is both sides
 .odstep = -1,
 .retries_l1 = -1,
 .retries_l2 = 3,
 .signature = "Not specified",
 .start = -1,
 .unattended = 0,
 .unattended_retry_abort_max = 300,
 .unattended_retry_sector_max = 2,
 .unattended_seeked_max = 10,
 .verbose = 1
};

dg_opts_t dg_opts =
{
 .sidedness = -1,
 .cylinders = -1,
 .heads = -1,
 .sectors = -1,
 .secbase = -1, 
 .secsize = 0,
 .idatarate = -1,
 .odatarate = -1, 
 .rwgap = -1,
 .fmtgap = -1,
 .ifm = -1,
 .ofm = -1, 
 .nomulti = -1,
 .noskip = -1,
 .sideoffs = -1,
 .side1as0 = -1, 
 .secbase1s = -1,
 .secbase2s = -1,
 .secbase2c = -1,
 .skew_ofs = -1,
 .skew_val = -1,
 .special = -1
};

xdg_t xdg =
{
 .dg_secbase2c = -1
};

static char *datarates_str[] =
{
 "hd",      // Data rate for 1.4Mb 3.5"  in 3.5"  drive
 "dd",      // Data rate for 360k  5.25" in 1.2Mb drive
 "sd",      // Data rate for 720k  3.5"  in 3.5"  drive
 "ed"       // Data rate for 2.8Mb 3.5"  in 3.5"  drive
};

static char *encmode_str[] =
{
 "mfm",
 " fm"
};
 
static char *sidedness[] =
{
 "alt",     // Track n is cylinder (n/heads) head (n%heads)
 "outback", // Tracks go (head 0) 0,1,2,3,...37,38,39, then
            // (head 1) 39,38,37,...,2,1,0
 "outout"   // Tracks go (head 0) 0,1,2,3,...37,38,39, then
            // (head 1) 0,1,2,3,...37,38,39
};

// data rates (must match LibDsk RATE_* enumerations)
static const int data_rates_val[] =
{
 500000,
 300000,
 250000,
 1000000,
};

//==============================================================================
// These format tables along with sector size and number of sectors are used
// for determing the GAP4 amount if calculate mode is used.
//
// GAP4 is calculated, overriden or uses dg_fmtgap values.
//
// The LibDsk floppy driver currently is not able to alter the other GAP or
// SYNC quantities so the GAP4 is basically determined by sector size,
// number of sectors and a set number of GAP and SYNC bytes.
//
// Remote Floppy drivers such as Floppyio does it's own GAP4 calculations
// and is also able to vary the other GAP and SYNC quantities.  The GAP4
// value calculated here will only be used by Floppyio if it is told to do
// so by specifying --gapset=0x3ffe.  If doing so the other GAP and SYNC
// values should remain unchanged.  It is not known how a PC floppy driver
// determines the GAP and SYNC quanity values.
//
// The values in square brackets below are the minimum recommended values
// found in the WD2793 data sheet.
//
// The IBM 3740 format uses the values as show in the WD2793 data sheet for
// a 26x128 disk.
//
// The IBM System 34 format uses values that are suitable for a 9x512 or
// 10x512 disk with the values as defined in the Atari reference link:
// http://info-coach.fr/atari/software/FD-Soft.php#fd_soft_standard_512B_formats
//
// GAP 5 is determined by the floppy controller which keeps writing GAP
// bytes until it interrupts out.
//==============================================================================
//
// IBM 3740 track format
const fdc_format_data_t ibm_3740[] =
{
 {FDC_FMT_GAP1,   0xff,    40},  // GAP #1                [16] -----+
 {FDC_FMT_SYNC1,  0x00,     6},  // SYNC #1                [?]      |
 //                                                                 | 
 // track header                                                    |  ONCE
 {FDC_FMT_IM,     0xfc,     1},  // Index Mark                      |
 //                                                                 |
 {FDC_FMT_GAP2,   0xff,    26},  // GAP #2                [11] -----+

 // sector ID record (once per sector)
 {FDC_FMT_SYNC2,  0x00,     6},  // SYNC #2                [6] -----+
 {FDC_FMT_IDAM,   0xfe,     1},  // ID Address Mark                 |
 {FDC_FMT_TRACK,  0x00,     1},  // track number                    |
 {FDC_FMT_SIDE,   0x00,     1},  // side number (0 or 1)            |
 {FDC_FMT_SECTOR, 0x00,     1},  // sector number                   |
 {FDC_FMT_LENGTH, 0x00,     1},  // sector length (0,1,2 or 3)      |
 {FDC_FMT_CRC,    0xf7,     1},  // writes 2 CRCs                   |
 //                                                                 |
 {FDC_FMT_GAP3,   0xff,    11},  // GAP #3                [10]      |    X
 {FDC_FMT_SYNC3,  0x00,     6},  // SYNC #3                [4]      | SECTORS
 //                                                                 |
 // sector data field record (once per sector)                      |
 {FDC_FMT_DAM,    0xfb,     1},  // Data Address Mark               |
 {FDC_FMT_DATA,   0xe5,   128},  // data (i.e 0xe5)                 |
 {FDC_FMT_CRC,    0xf7,     1},  // writes 2 CRCs                   |
 //                                                                 | 
 {FDC_FMT_GAP4,   0xff,     0},  // GAP #4                [16] -----+

 {FDC_FMT_GAP5,   0xff, 32768}   // GAP #5 (determined by FDC) -----+  ONCE
};

// IBM SYSTEM 34 track format
const fdc_format_data_t ibm_system_34[] =
{
 {FDC_FMT_GAP1,   0x4e,    60},  // GAP #1                [32] -----+
 {FDC_FMT_SYNC1,  0x00,    12},  // SYNC #1                [?]      |
 //                                                                 |
 // track header                                                    |
 {FDC_FMT_F6,     0xf6,     3},  // writes 3 x 0xC2                 |  ONCE
 {FDC_FMT_IM,     0xfc,     1},  // Index Mark                      |
 //                                                                 |
 {FDC_FMT_GAP2,   0x4e,    22},  // GAP #2                [22] -----+

 // sector ID record (once per sector)
 {FDC_FMT_SYNC2,  0x00,    12},  // SYNC #2               [12] -----+
 {FDC_FMT_F5,     0xf5,     3},  // writes 3 x 0xA1                 |
 {FDC_FMT_IDAM,   0xfe,     1},  // ID Address Mark                 |
 {FDC_FMT_TRACK,  0x00,     1},  // track number                    |
 {FDC_FMT_SIDE,   0x00,     1},  // side number (0 or 1)            |
 {FDC_FMT_SECTOR, 0x00,     1},  // sector number                   |
 {FDC_FMT_LENGTH, 0x00,     1},  // sector length (0,1,2 or 3)      |
 {FDC_FMT_CRC,    0xf7,     1},  // writes 2 CRCs                   |
 //                                                                 |
 {FDC_FMT_GAP3,   0x4e,    22},  // GAP #3                [24]      |    X
 {FDC_FMT_SYNC3,  0x00,    12},  // SYNC #3                [8]      | SECTORS
 //                                                                 |
 // sector data field record  (once per sector)                     |
 {FDC_FMT_F5,     0xf5,     3},  // writes 3 x 0xA1                 |
 {FDC_FMT_DAM,    0xfb,     1},  // Data Address Mark               |
 {FDC_FMT_DATA,   0xe5,   256},  // data (i.e 0xe5)                 |
 {FDC_FMT_CRC,    0xf7,     1},  // writes 2 CRCs                   |
 //                                                                 |
 {FDC_FMT_GAP4,   0x4e,     0},  // GAP #4                [16] -----+

 {FDC_FMT_GAP5,   0x4e, 32768}   // GAP #5 (determined by FDC) -----+  ONCE
};

// format names (must be in same order as enumerated!)
char *fdc_fmt_id_names[] =
{
 "GAP1",
 "GAP2",
 "GAP3",
 "GAP4",
 "GAP5", 

 "SYNC1",
 "SYNC2",
 "SYNC3",

 "TRACK",
 "SIDE",
 "SECTOR",
 "LENGTH",
 "DATA",

 "F5",
 "F6",
 "IM",
 "IDAM",
 "DAM", 
 "CRC"
}; 

static char *noyes[] =
{
 "no",
 "yes"
};

static char *sides[] =
{
 "both",
 "0",
 "1"
};

static char *forceside_values[] =
{
 "off",
 "on",
 "00",
 "01",
 "10",
 "11",
 ""
};

static char *is_floppy_otypes[] =
{
 "floppy",
 "ntwdm",
 "remote",
 "rcpmfs",
 ""
};

static char *is_not_var_track_size_type[] =
{
 "raw",
 "dsk",
 ""
};

static char *no_error_file[] =
{
 "floppy",
 "ntwdm",
 "remote",
 "rcpmfs",
 ""
};

static char *no_info_file[] =
{
 "floppy",
 "ntwdm",
 "remote",
 "rcpmfs",
 ""
};

static char *no_desc_otypes[] =
{
 "floppy",
 "ntwdm",
 "remote",
 "rcpmfs",
 ""
};

sup_t input_sup;
sup_t output_sup;

FILE *infof;

static FILE *errorf;

static int sect_retry_count;
static int sect_retries_tot;
static int sect_errors_tot;

static int auto_retry_abort;
static int auto_retry_sector;
static int auto_seeked_count;
static int auto_cyl_last;
static int auto_head_last;
static int auto_move_head;

char userhome[SSIZE1];
char userhome_confpath[SSIZE1];
static char userhome_path[SSIZE1];
static char error_file[1000];

DSK_GEOMETRY dg;
DSK_GEOMETRY cdg;
DSK_PDRIVER idrive = NULL;

static DSK_PDRIVER odrive = NULL;

static info_t info;

static int overwrite_flag;

static int cyl_start;
static int cyl_finish;
static int trk_start;
static int trk_finish;

static int skip_all_errors;

static int skew_table[1024];
static uint8_t buf[100000];
static int buffered_cylinder;
static int buffered_head;

static char ofile_name[1000];

// chars = DESC_CHARS + 1 CR + 1 null
static char description[DESC_LINES+1][DESC_CHARS+2]; 

static int diskdesc_line;

#ifdef WIN32
extern char *c_argv[];
extern int c_argc;
#endif

extern int exitstatus;
extern char *detect_disk_args[];
extern char config_file[SSIZE1];
extern char config_vers[];

//==============================================================================
// Prototypes (static)
//==============================================================================
static dsk_err_t home_and_reset_input_drive_and_settings (DSK_FORMAT *sector_id);
static int disk_clean (int method, int cyl_last);
static int open_input_drive (void);

//==============================================================================
// Report DSK_GEOMETRY values.
//
// This also reports the extended ubeedisk xdg->dg_idatarate,
// xdg->dg_odatarate, xdg->dg_ifm and xdg->dg_ofm values.  These values are
// used elsewhere to set dg->dg_datarate and dg->dg_fm as needed.
//
//   pass: DSK_GEOMETRY *dg
// return: void
//==============================================================================
void report_dg (DSK_GEOMETRY *dg)
{
 // dump the DSK_GEOMETRY structure
 printf("\nreport_dg()\n"
 "sidedness:     %d\n"
 "cylinders:     %d\n"
 "heads:         %d\n"
 "sectors:       %d\n"
 "secbase:       %d\n"
 "secsize:       %d\n"
 "datarate:      %s\n"
 "xdg idatarate: %s\n"
 "xdg odatarate: %s\n" 
 "rwgap:         %d\n"
 "fmtgap:        %d\n"
 "fm:            %s\n"
 "xdg ifm:       %s\n"
 "xdg ofm:       %s\n"
 "nomulti:       %d\n"
 "noskip:        %d\n",
 dg->dg_sidedness,
 dg->dg_cylinders,
 dg->dg_heads,
 dg->dg_sectors,
 dg->dg_secbase,
 dg->dg_secsize,
 datarates_str[dg->dg_datarate],
 datarates_str[xdg.dg_idatarate],
 datarates_str[xdg.dg_odatarate],
 dg->dg_rwgap,
 dg->dg_fmtgap,
 encmode_str[dg->dg_fm],
 encmode_str[xdg.dg_ifm],
 encmode_str[xdg.dg_ofm],
 dg->dg_nomulti,
 dg->dg_noskip);
}

//==============================================================================
// Report callback function.
//
//   pass: const char *s
// return: void
//==============================================================================
static void report (const char *s)
{
 if (disk.verbose < 2)
    return;
 printf("%s\n", s);
}

//==============================================================================
// Report callback end function.
//
//   pass: void
// return: void
//==============================================================================
static void report_end (void)
{
 if (disk.verbose < 2)
    return;
 printf("\r%-79.79s\r", "");
 fflush(stdout);
}

//==============================================================================
// Ask user for overwrite permission if destination disk file already exists.
//
// If the destination file exists prompt the user for the required action.
// This may be 1 of the following:
//
// [N]o       : do not overwrite this one file.
// [Y]es      : overwrite this one file.
// [A]ll      : overwrite this and all other existing files.
// [S]kip all : do not overwrite this or any other files.
//
// The last 2 are only used if copying more than one disk.
//
// Before calling this function set overwrite_flag=-1 each time the copy
// process is called.
//
//   pass: int action                   1=single file prompts
//                                      2=multiple file prompts
//         char *destfile               path to file to be checked
// return: int                          0=no, 1=yes, 2=skip all
//==============================================================================
static int overwrite_permission (int action, char *destfile)
{
 char user[100];

 // don't prompt the user for action if the --force option was specified
 if (disk.force)
    return 1;

 // always return 1 if it's a floppy type (non disk image)
 if (string_search(is_floppy_otypes, disk.otype) != -1)
    return 1;
 
 // if the user selected [A]ll or [S]kip last time
 if (overwrite_flag != -1)
    return overwrite_flag;

 // see if the file can be opened if it exists
 FILE *fp = fopen(destfile, "r");

 // return if file does not exist
 if (! fp)
    return 1;
    
 fclose(fp);

 for (;;)
    {
     printf(APPNAME": Destination file already exists. Overwrite \"%s\" ? %s",
     destfile, (action > 1)? "[N]o [Y]es [A]ll overwrite [S]kip all: ":
     "[N]o [Y]es: ");
     fflush(stdout);
 
     if (fgets(user, sizeof(user)-2, stdin))
        {} // ignore result 

     switch (toupper(user[0]))
        {
         case 'N' : // no overwrite
            return 0;
         case 'Y' : // overwrite 1
            return 1;
         case 'A' : // overwrite all
            if (action > 1)
               {
                overwrite_flag = 1;
                return 1;
               }
            break;
         case 'S' : // skip all
            if (action > 1)
               {
                overwrite_flag = 2;
                return 2;
               }
            break;
        }
    }
}

//==============================================================================
// Check if user is requesting the pause menu and act on selections.
//
// Allow the operator to pause, abort, clean heads and switch between
// unattended and interactive modes depending on requested options.
//
// The 'cyl' value will be used by the clean process to place the head on
// the current cylinder before the clean process was run.
//
//   pass: int options                  0=+RACM, 1=+RAC-M, 2=+RA-CM
//         int cyl_last                 last cylinder position
// return: int                          result value
//==============================================================================
static int pause_menu (int options, int cyl_last)
{
 char user[100];

 if (get_key() == -1)
    return DSK_ERR_OK;
    
 user[0] = 0;
 printf("\n");         
 while (toupper(user[0]) != 'R')
    {
     printf("\nPAUSE MENU\n");
     printf("----------\n");

     switch (options)
        {
         case 0 : 
            if (disk.unattended)
               printf("[R]esume [A]bort [C]lean [M]ode set to interactive ?: ");
            else
               printf("[R]esume [A]bort [C]lean [M]ode set to unattended ?: ");
            break;
         case 1 : 
            printf("[R]esume [A]bort [C]lean ?: ");
            break;
         case 2 : 
            printf("[R]esume [A]bort ?: ");
            break;
        }
         
     fflush(stdout);

     if (fgets(user, sizeof(user)-2, stdin))
        {} // ignore result 

     switch (toupper(user[0]))
        {
         case 'A' : // abort the copy (not an error from this menu)
            return DSK_ERR_ABORT;                 
         case 'C' : // clean drive heads with special disk
            if (options > 1)
               break;
            disk_clean(0, cyl_last);
            break;
         case 'R' : // resume
            break;
         case 'M' : // switch between unattended and interactive mode
            if (options != 0)
               break;
            disk.unattended = ! disk.unattended;
            if (disk.unattended)
               printf("Unattended mode set!\n");
            else
               printf("Interactive mode set!\n");
            break;
         default :
            break;
        }
    }

 printf("\n");
 return DSK_ERR_OK;  
}

//==============================================================================
// Log a read error.
//
// Logs read errors if 'info' files are enabled and the log level is not 0.
// At completion a '.err' file will be created or may be optionally appended
// to the '.info' file if requested.
//
// The error file is only created if there were errors encountered.
//
//   pass: dsk_err_t dsk_err            LibDsk disk error
//         int tryx                     try number
//         dsk_pcyl_t cyl
//         dsk_phead_t head
//         int lsect                    logical sector number
// return: void
//==============================================================================
static void log_read_error (dsk_err_t dsk_err, int tryx, dsk_pcyl_t cyl,
                            dsk_phead_t head, int lsect)
{
 if (! disk.info_file || ! disk.log ||
    string_search(no_error_file, disk.otype) != -1)
    return;

 if (! errorf)
    {
     // create the error log file
     snprintf(error_file, sizeof(error_file), "%s.err", ofile_name);
     error_file[sizeof(error_file)-1] = 0;

     errorf = fopen(error_file, "w+");

     if (errorf == NULL)
        {
         printf(APPNAME": unable to create error file: %s\n", error_file);
         return;
        }
     else
        {
         fprintf(errorf, "LOG OF READ ERRORS\n");
         fprintf(errorf, "------------------\n");

         fprintf(errorf,
 "This log contains errors that were encountered during the imaging process.\n"
 "Read errors may have been corrected on further retries.\n"
 "\n"
 "The sector count at 'Sector errors' and the 'SECTOR STATUS MAP' in the\n"
 "'info' file should be used to determine what bad sectors the disk (image)\n"
 "may have.\n"
 "\n"
 "The frequency of log entries recorded here is determined by the logging\n"
 "level option '--log=n'.\n"
 "\n");
        }
    }

 if (errorf)
    {
     fprintf(errorf,
     "Try #: %02d/%02d Cylinder: %02d Head: %d Sector: %03d Error: (%d) %s\n",
     tryx, disk.retries_l2, cyl, head, skew_table[lsect] + dg.dg_secbase,
     dsk_err, dsk_strerror(dsk_err));
    }
}

//==============================================================================
// Calculate the track size.
//
// Where 0xf7 (CRC) is used 2 bytes will be added to the count accumulator.
//
// GAP 1, 2 and 3
// --------------
// Uses the format table values.

// SYNC 1, 2 and 3
// ---------------
// Uses the format table values.
//
// GAP 4
// -----
// If fmtgap is 0 then the GAP4 value will be calculated later using:
//
// GAP 4 = NOMINAL TRACK SIZE - TRACK COUNT / SECTORS PER TRACK
//
// GAP 5
// -----
// There is no setting for this.  The floppy controller keeps writing gap
// bytes until an Interrupt out condition occurs.
//
//   pass: fdc_format_data_t *fmtd      format data
//         DSK_FORMAT *format           information for 1 track
//         int fmtgap                   0, GAPSET(4) or dg_fmtgap
//         int sectors                  number of sectors
// return: int                          total byte count, else -1 if error
//==============================================================================
static int format_track_size (fdc_format_data_t *fmtd,
                              DSK_FORMAT *format, int fmtgap, int sectors)
{
 fdc_format_data_t *p = fmtd;
 fdc_format_data_t *fmtd_sect = p;

 int id; 
 int count = 0;
 int buf_count = 0;

 if (disk.verbose > 1)
    printf("format_track_size():\n"); 

 while (sectors)
    {
     id = p->id;

     switch (p->id)
        {
         case FDC_FMT_GAP1:
            count = p->count;
            break;
         case FDC_FMT_SYNC1:
            count = p->count;
            break;
         case FDC_FMT_GAP2:
            count = p->count;
            break;
            
         case FDC_FMT_SYNC2:
            count = p->count;
            fmtd_sect = p - 1;  // save format table pointer (to here - 1)
            break;
         case FDC_FMT_GAP3:
            count = p->count;
            break;
         case FDC_FMT_SYNC3:
            count = p->count;
            break;
         case FDC_FMT_GAP4:
            count = fmtgap;
            p = fmtd_sect;  // restore format table pointer for sector
            format++;  // next sector structure
            sectors--;
            break;
            
         case FDC_FMT_DATA:
            count = format->fmt_secsize;
            break;
         case FDC_FMT_CRC:
            count = 2;     // 2 CRCs are written for this by the FDC
            break;
         default:  // all other values come here
            count = p->count;
            break;
        }

     buf_count += count;     

     if (disk.verbose > 2)
        printf("ID=%s secs=%d bufc=%d count=%d\n",
        fdc_fmt_id_names[id], sectors, buf_count, count);
        
     p++;
    }

 // we don't have a track format buffer so no need for this here
 // if (buf_count > BUFSIZE)
 //    return -1;
    
 return buf_count;
}

//==============================================================================
// Dynamic calculation of GAP4 values based on drive type, data rates, sector
// size, sectors/track, density and other GAP and SYNC values.
//
// This code has been taken from a closed source (Floppyio firmware) and
// modified as needed.  It's only job is to calculate GAP values based on
// IBM 3740 and IBM system 34 formats.  The actual bytes written by LibDsk
// and host drivers may differ to that of the Floppyio firmware but should
// be similar enough for the calculations to produce the required GAP
// values.
//
// This function will be called for both floppy and disk images.  A disk
// image will have a drive type of 0 and a floppy type will be one of the
// following types: 'd', 'h', 'D' or 'H'.  If the output drive has a type of
// 0 then the input drive type will be used instead for calculations.  If
// both the input and output type are 0 then the calculations will be based
// on the data rate for the format.
//
// The size of a track will depend on the data rate value which also
// determines the drive type in use and density values (DD/SD).  The nominal
// amount of data for a track will be halved if FM mode is used.  FM mode
// assumes the controller is running at halve the data rate but it also
// determines what format algorithm to be used.  The data rate value can't
// be used for this.
//
// Speed variations of drives and the accuracy of the FDC clock input will
// ultimately determine how much data will fit onto one track.  A track when
// written starts from the index pulse and continues until the next index
// pulse is detected.  If the drive is spinning faster or the FDC data rate
// is slower than it should be then the last sector (and it's gap) may not
// fit on a track.  When reading and writing individual sectors this is not
// generally a problem as the disk determines where each sector starts
// (synced to data) but a track MUST be formatted with some safety margin. 
// A figure of X% will be used to reduce the nominal track size when
// calculating how much gap to use.  The amount of gap written after the
// last sector in the format depends on when the index pulse is detected. 
// If the value for GAP4 is too low then there may be problems.
//
// If the 300 kHz data rate is used then the calculated track size is the
// same as for the 250 kHz data rate.  This is because 300 kHz is used for
// drives that are 6 revs/sec and 250 kHz for 5 revs/sec.
//
// IBM 3740 format will be used for single density (FM) and IBM system 34
// format for double density (MFM).
//
// drate values
// ============
// RATE_HD High-density disc (1.4Mb or 1.2Mb)
// RATE_DD Double-density disc in 1.2Mb drive (ie, 360k disc in 1.2Mb drive)
// RATE_SD Double-density disc in 1.4Mb or 720k drive
// RATE_ED Extra-density disc (2.8Mb)
//
// Double density = DD, Single density = SD
//
// RATE_HD (FDC_500KHZ)
// ====================
// Will need to determine if a 3.5" or 5.25/8" is in use from the passed
// 'type' parameter as the RPM is different for these two types.
//
// 5.25" HD and 8" drives
// ----------------------
// 5.25" HD and 8" drives rotate at 360 RPM and use a 500kHz data rate for
// HD.  The nominal track size for DD is 10416 bytes = (500000 / 8) / 6 and
// half that if SD is being used (5208).
//
// 3.5" drive
// ----------
// 3.5" drives rotate at 300 RPM and use a 500kHz data rate if the disk is a
// HD type.  The nominal track size for DD is 12500 bytes = (500000 / 8) / 5
// and half that if SD is being used (6250).
//
// RATE_DD (FDC_300KHZ)
// ====================
// 5.25" and 8" drives
// -------------------
// 5.25" HD and 8" drives rotate at 360 RPM and use a 300kHz (5.25") data
// rate for DD media.  The nominal track size for DD is 5208 bytes = (250000
// / 8) / 6 and half that if SD is being used (2604).
//
// RATE_SD  (FDC_250KHZ)
// =====================
// 5.25" DD and 3.5" drives
// ------------------------
// 5.25" DD and 3.5" drives rotate at 300 RPM and use a 250kHz data rate for
// DD media.  The nominal track size for DD is 6250 bytes = (250000 / 8) / 5
// and half that if SD is being used (3125).
//
// RATE_ED (FDC_1000KHZ)
// =====================
// Not currently supported.
//
//   pass: DSK_FORMAT *format           information for 1 track
// return: int                          0 else -1 if error
//==============================================================================
static int fdc_buffer_format (DSK_FORMAT *format)
{
 int track_nominal;
 int track_calc;
 int track_max;
 int dt;
 int fmtgap = dg.dg_fmtgap;   

 fdc_format_data_t *p;

 if (disk.verbose > 1)
    printf("fdc_buffer_format()\n");
    
 // if the input and output drive types are both type 0 then we take a
 // calculated guess as to what the drive type might be.  It's only needed
 // to find the REVs value.  If the geometry rate is RATE_DD then we assume
 // a DD disk in a 5.25" 1.2Mb drive which is type 'h' and 6 revs/sec
 // otherwise all others assumed to be type 'H' and 5 revs/sec

 if (disk.odrive_type == 0 && disk.idrive_type == 0)
    {
     if (xdg.dg_odatarate == RATE_DD)
        dt = 'h';
     else
        dt = 'H';
    }
 else
    {
     // if the output drive type is not 0 then use the output type value
     // otherwise use the input to determine the output floppy drive type
     // (both values won't be 0 here)
     if (disk.odrive_type == 0)
        dt = disk.idrive_type;     
     else
        dt = disk.odrive_type;
    }

 if (disk.verbose > 1)    
    printf("drive-type=%c datarate=%s rec-mode=%s fmtgap=%d sectors=%d\n",
    dt, datarates_str[xdg.dg_odatarate],
    dg.dg_fm? "fm":"mfm", dg.dg_fmtgap, dg.dg_sectors);

 // nominal bytes per track (10416/12500/5208/6250) at full density
 track_nominal = (data_rates_val[xdg.dg_odatarate] / 8) / ((dt == 'h')? 6:5);

 // if FM mode then halve the amount of data that will fit onto a track
 if (dg.dg_fm)
    track_nominal /= 2;

 // reduce the nominal track size to make sure a complete track will fit due
 // to variations of drives, a value of 2.5% means a 300 RPM drive may spin
 // at 292.5 - 307.5 RPM
 track_max = track_nominal - (track_nominal * (2.5 / 100.0));

 // pointer to the FM or MFM format data
 p = (dg.dg_fm == 1) ? (fdc_format_data_t *)ibm_3740 :
 (fdc_format_data_t *)ibm_system_34;

 // determine early GAP4 value if value known in advance
 if (disk.gap_set[GAP4] == -1 || disk.gap_set[GAP4] == FDC_AUTO_GAP)
    fmtgap = 0;
 else
    if (disk.gap_set[GAP4] != FDC_DG_FMTGAP)
       fmtgap = disk.gap_set[GAP4];
                   
 // calulate the track size
 track_calc = format_track_size(p, format, fmtgap, dg.dg_sectors);

 if (track_calc == -1)
    {
     printf("Error: format_track_size() returned an error code\n");
     return -1;
    }

 // track size calculation used a 0, GAPSET(4), or dg_fmtgap value, if
 // if it is 0 we calculate the actual GAP4 value now
 if (fmtgap == 0)
    {
     // find gap value
     fmtgap = (track_max - track_calc) / dg.dg_sectors;
                 
     // adjust the size value for final GAP
     track_calc += (fmtgap * dg.dg_sectors);
                                
     if (disk.verbose > 1)
        printf("Using AUTO GAP method for GAP4 - fmtgap=%d track_calc=%d\n",
        fmtgap, track_calc);
    }
 else
    {
     // overall track size already calculated, the fmtgap value is the
     // one passed to this function or we used a GAPSET(4) value
     if (disk.verbose > 1)
        printf("Using %s method for GAP4 - fmtgap=%d track_calc=%d\n",
     disk.gap_set[GAP4] == FDC_DG_FMTGAP ? "dg_fmtgap":"GAPSET",
     fmtgap, track_calc);
    }

 // plug the 'fmtgap' value back into the disk geometry     
 dg.dg_fmtgap = fmtgap;
 
 // what do we do with dg_rwgap value? we had set it to same value as the
 // calculated dg_fmtgap before but it is not clear if it is ever used
 if (dg_opts.rwgap == -1) 
    dg.dg_rwgap = fmtgap;
 
 if (disk.verbose > 1)
    printf("track_nominal=%d usable track size=%d actual track_calc=%d\n",
    track_nominal, track_max, track_calc);

 // the size must not be bigger than usable track size
 if (track_calc > track_max)
    if (disk.verbose > 1)
        printf("Warning: format track_calc=%d larger than track size=%d\n",
        track_calc, track_max);
 
 return 0;
}    

//==============================================================================
// Set the values in the 'format' structure for a single track based on the
// passed cylinders, head and side values.
//
// A format operation will call this for each track to be formatted.  A read
// operation may only call it once so that the GAP values can be calculated.
//
//   pass: dsk_pcyl_t cyl               cylinder number
//         dsk_phead_t head             physical side of disk
//         int side                     side ID value
// return: int                          0 else -1 if error
//==============================================================================
static dsk_err_t set_format_struct (dsk_pcyl_t cyl, dsk_phead_t head, int side,
                                    DSK_FORMAT *format)
{
 int lsect;
 int psect = 0;
 int skewing;
 int floppy_type = (string_search(is_floppy_otypes, disk.otype) != -1);

 if (disk.verbose > 1)
    printf(" Physical sectors: "); 

 for (lsect = 0; lsect < dg.dg_sectors; lsect++)
    {
     format[lsect].fmt_cylinder = cyl;
     format[lsect].fmt_head = side;
     skewing = 0;

     // only allow physical skewing for outputs that are floppy types
     if (floppy_type)
        {
         // physical sector skewing call-back function if defined
         if (xdg.pss_cb)
            if ((psect = (*xdg.pss_cb)(cyl, head, lsect)) != -1)
                skewing = 1;
     
         // or it may have skew data defined for sides 0 and/or 1          
         if (xdg.dg_pskew0 || xdg.dg_pskew1)
            {
             skewing = 1;
             psect = (head == 0)?
             *(xdg.dg_pskew0 + lsect):*(xdg.dg_pskew1 + lsect);
            }
        }

     if (! skewing)
        {
         if (xdg.dg_secbase2c != -1 && cyl >= xdg.dg_secbase2c)
            psect = xdg.dg_secbase2s + lsect;
         else   
            psect = xdg.dg_secbase1s + lsect;
        }

     format[lsect].fmt_sector = psect;
     format[lsect].fmt_secsize = dg.dg_secsize;
     
     if (disk.verbose > 1)
        printf("%d ", psect);
    }

 if (disk.verbose > 1)
    printf("\n");    

 return fdc_buffer_format(format);
}

//==============================================================================
// Format a disk track.
//
// Disk images will use sequential order sector numbers regardless of any
// physical skewing associated with a format type.  Physical skewing will be
// used if the output type is floppy based and the format type requires it.
//
//   pass: dsk_pcyl_t cyl               cylinder number
//         dsk_phead_t head             physical side of disk
//         int side                     side ID value
// return: dsk_err_t                    DSK_ERR_OK if successful
//==============================================================================
static dsk_err_t format_track (dsk_pcyl_t cyl, dsk_phead_t head, int side)
{
 dsk_err_t dsk_err = DSK_ERR_OK;

 DSK_FORMAT format[256];

 // set the 'format' structure for a track
 if (set_format_struct(cyl, head, side, format) == -1)
    dsk_err = DSK_ERR_UNKNOWN;
 else
    {
     // avoid dsk_pformat() for formats that do not support the function
     if (! output_sup.pformat)
        return DSK_ERR_OK;

     // format one track
     dsk_err = dsk_pformat(odrive, &dg, cyl, head, format, 0xe5);

     // ignore formatting if the driver does not support the format function.
     if (dsk_err == DSK_ERR_NOTIMPL)
        return DSK_ERR_OK;
    }

 // report the error 
 if (dsk_err != DSK_ERR_OK)
    {
     printf("\n"APPNAME
     ": format_track() - Cylinder: %02d Head: %d Side: %03d Error: (%d) %s\n",
     cyl, head, side, dsk_err, dsk_strerror(dsk_err));
    }

 return dsk_err;   
}

//==============================================================================
// Format a range of disk tracks.
//
// This done using logical tracks method.
//
//   pass: int start                    logical track start number
//         int finish                   logical track finish number 
// return: dsk_err_t                    DSK_ERR_OK if successful
//==============================================================================
static dsk_err_t format_track_range (int start, int finish)
{
 dsk_err_t dsk_err = DSK_ERR_OK;
 dsk_pcyl_t cyl;
 dsk_ltrack_t trk;
 dsk_phead_t head = 0;

 // format tracks if start > cylinder 0
 if (disk.noformat)
    return DSK_ERR_OK;

 if (disk.verbose)
    printf("Formatting tracks: %d to %d.\n", start, finish);

 for (trk = start; trk <= finish; trk++)
    {
     cyl = trk / dg.dg_heads;
     head = trk % dg.dg_heads;

     if (disk.verbose)
        {
         printf("\rCylinder: %02d/%02d Head: %d/%d",
                cyl, dg.dg_cylinders-1, head, dg.dg_heads-1);
         fflush(stdout);
        } 
     dsk_err = format_track(cyl, head, head + xdg.dg_sideoffs);
    }

 if (dsk_err != DSK_ERR_OK)
    {
     printf("\nWrite error: %s\n", dsk_strerror(dsk_err));
     printf("Aborting this disk copy.\n");
     return dsk_err;
    }

 if (disk.verbose)
    printf("\n");

 return dsk_err;
}

//==============================================================================
// Generate skew data for reading sectors based on the parameters passed.
//
//   pass: int skew                     skewing value
//         int start                    first logical sector in table
//         int sectors                  number of sectors per track
// return: void
//==============================================================================
static void create_skew_table (int skew, int start, int sectors)
{
 int i;
 int k;
 int j = start;
 
 for (i = 0; i < sectors; ++i)
    {
     for (;;)
        {
         for (k = 0; k < i && skew_table[k] != j; ++k)
            ;
    
         if (k < i)
            j = (j + 1) % sectors;
         else
            break;
        }
    
     skew_table[i] = j;
     
     j = (j + skew) % sectors;
    }
}

//==============================================================================
// Read a complete track of data into a global buffer.
//
// All tracks are buffered into memory.
//
// The sectors will be placed into the buffer in sequential physical sector
// order regardless of any physical skewing that may have been used or the
// ubeedisk skewing used.
//
// Disk reading time can be greatly reduced by reading a complete track into
// memory then writing it out.  It was found that reading every alternate
// sector (skew=2) starting with the 2nd sector on disks that have a
// sequential 512 byte sectors format works well.  It depends on how much
// gap exist between sectors and if enough a skew of 1 may work well.
//
// Note: This skewing arrangement has NOTHING to do with skewing that a
// system might use.  It's only purpose is for speeding up reading data from
// a floppy under ubeedisk.
//
// If the track read fails then the calling process should then manage the
// reading of all the sectors on the track concerned.
//
// To see the skew values use --verbose=2 on the command line.
//
// Below are various skew value combinations for the Microbee DS40 format. 
// The 1,2 (default) method requires 3 disk rotations to read in a complete
// track.  In this case the 1st sector is read last.
//
// disk.skew_ofs = 1 and disk.skew_val = 2 (default)
// 2  4  6  8 10  3  5  7  9  1
//
// disk.skew_ofs = 0 and disk.skew_val = 2
// 1  3  5  7  9  2  4  6  8 10
//
// disk.skew_ofs = 0 and disk.skew_val = 1
// 1  2  3  4  5  6  7  8  9 10
//
//
//   pass: dsk_pcyl_t cyl               physical drive cylinder number
//         dsk_pcyl_t xcyl              ID cylinder value
//         dsk_phead_t head             physical drive side
//         dsk_phead_t xhead            ID side value
// return: dsk_err_t                    DSK_ERR_OK if successful
//==============================================================================
static dsk_err_t read_buffered_track (dsk_pcyl_t cyl, dsk_pcyl_t xcyl,
                                      dsk_phead_t head, dsk_phead_t xhead)
{
 // enable this to speed up testing and use --verbose=2 --finish=0 --heads=1
 // on the command line.
 uint8_t *p;
 int psect;
 int i;
 dsk_err_t implemented = DSK_ERR_OK;
 dsk_err_t dsk_err = DSK_ERR_OK;

 //printf("read_buffered_track()\n");

 // avoid dsk_xread() for types that do not support the function
 if (! input_sup.xread)
    implemented = DSK_ERR_NOTIMPL;

 // read in the complete track without any console IO (unless --verbose > 1)
 for (i = 0; i < dg.dg_sectors; i++)
    {
     dsk_err = implemented;

     // get the skewed physical sector number
     psect = skew_table[i] + dg.dg_secbase;

     // calculate the buffer location where the sector data will be placed
     p = buf + dg.dg_secsize * (psect - dg.dg_secbase);

     // skip sector read call-back function if defined
     if (! xdg.ssr_cb || (xdg.ssr_cb && ((*xdg.ssr_cb)
     (p, cyl, head, psect) == 0)))
        {
         // else read the sector
         if (dsk_err != DSK_ERR_NOTIMPL)
            dsk_err = dsk_xread(idrive, &dg, p, cyl, head, xcyl, xhead, psect,
            dg.dg_secsize, NULL);

         if (dsk_err == DSK_ERR_NOTIMPL)
            dsk_err = dsk_pread(idrive, &dg, p, cyl, head, psect);

         // abort the track read if an error (will use sector by sector)
         if (dsk_err != DSK_ERR_OK)
            break;
        }
    }

 // if error save the cylinder and head as negative values before returning
 if (dsk_err != DSK_ERR_OK)
    {
     buffered_cylinder = -2;  // flag it to use non buffered reads
     return dsk_err;
    } 

 // save the buffered locations
 buffered_cylinder = cyl;
 buffered_head = head;
 
 return dsk_err;
}

//==============================================================================
// Write a buffered track.
//
//   pass: dsk_pcyl_t cyl               physical drive cylinder number
//         dsk_pcyl_t xcyl              ID cylinder value
//         dsk_phead_t head             physical drive side
//         dsk_phead_t xhead            ID side value
// return: dsk_err_t                    DSK_ERR_OK if successful
//==============================================================================
static dsk_err_t write_buffered_track (dsk_pcyl_t cyl, dsk_pcyl_t xcyl,
                                       dsk_phead_t head, dsk_phead_t xhead)
{
 DSK_FORMAT format[256];
 uint8_t *p;
 int i;
 int psect;
 dsk_pcyl_t use_cyl;
 dsk_phead_t use_head = xhead;
 dsk_err_t dsk_err = DSK_ERR_OK;

 // set data rate and MFM/FM mode for output
 dg.dg_datarate = xdg.dg_odatarate;
 dg.dg_fm = xdg.dg_ofm;

 if (disk.verbose > 1)
    report_dg(&dg);
 
 // seems LibDsk DSK and RAW drivers do not support the --oside option and
 // only works for floppy access?  Provide a work around if setting --oside
 // causes an error using the normal method.
 if (disk.oside_not_support && disk.oside != -1)
    {
     head = disk.oside;
     xhead = disk.oside;  // not a perfect solution
    }

 // set the 'format' structure for this track
 if (set_format_struct(cyl, head, xhead, format) == -1)
    return DSK_ERR_UNKNOWN;
    
//#define DEBUG_BUFFERING 
#ifndef DEBUG_BUFFERING 
 for (i = 0; i < dg.dg_sectors; i++)
    {
     // get the skewed physical sector number
     psect = skew_table[i] + dg.dg_secbase;

     // calculate the buffer location of the sector data
     p = buf + dg.dg_secsize * (psect - dg.dg_secbase);
    
     // write the sector to the output file, avoid dsk_xwrite() for types
     // that do not support the function
     if (! output_sup.xwrite)
        dsk_err = DSK_ERR_NOTIMPL;
     else
        {
         use_cyl = cyl;

         switch (disk.forceside)
            {
             case 0 : // off
                use_cyl = xcyl;
                use_head = xhead;
                break;
             case 1 : // on
                use_head = head;
                break;
             case 2 : // 00
                use_head = 0;
                break;
             case 3 : // 01
                if (head == 0)
                   use_head = 0;
                else
                   use_head = 1;
                break;
             case 4 : // 10
                if (head == 0)
                   use_head = 1;
                else
                   use_head = 0;
                break;
             case 5 : // 11
                use_head = 1;
                break;
            }        

         dsk_err = dsk_xwrite(odrive, &dg, p, cyl, head, 
         use_cyl, use_head, psect, dg.dg_secsize, 0);
        }

     if (dsk_err == DSK_ERR_NOTIMPL)
        dsk_err = dsk_pwrite(odrive, &dg, p, cyl, head, psect);

     if (disk.verbose > 1)
        printf("%d ", psect);  fflush(stdout);

     if (dsk_err != DSK_ERR_OK)
        {
         if (++disk.write_error_count <= 10)        
             printf("\n"APPNAME": write_buffered_track() Cyl:%03d Head:%02d "
             "Error:%s\n", cyl, head, dsk_strerror(dsk_err));

         if (disk.write_error_count == 10)
            printf(APPNAME": write_buffered_track() Only the first 10 "
            "write errors will be reported.\n");
         
         return dsk_err;
        }
    }

 if (disk.verbose > 1)
    printf("\n dsk_err=%d", dsk_err);    

#endif
 
 return dsk_err;
}

//==============================================================================
// Read sector ID to find the side ID.
//
// If the 'raw' or any other drivers not supporting the dsk_psecid()
// function then the side ID (xhead) will be returned using the passed head
// value except if disk.side1as0 is set in which case 0 is returned instead. 
// This also applies if there is an error reading a sector ID.  This is
// allowed to return as OK and relies on further sector reading to report an
// error.
//
// A high LibDsk retry level is set here to help make sure there is plenty
// of time to find an ID.  A constant is used but is changed for the actual
// sector reads that may follow.
//
//   pass: dsk_pcyl_t cyl               cylinder number
//         dsk_phead_t head             physical side of disk
//         dsk_phead_t *xhead           head result
//         int *xsecsize                sector size result
// return: dsk_err_t                    DSK_ERR_OK
//==============================================================================
int read_sector_id (dsk_pcyl_t cyl, dsk_phead_t head,
                    int *xhead, int *xsecsize)
{
 dsk_err_t dsk_err = DSK_ERR_OK;
 DSK_FORMAT result;

 // set data rate and MFM/FM mode for input
 dg.dg_datarate = xdg.dg_idatarate;
 dg.dg_fm = xdg.dg_ifm;

 dsk_set_retry(idrive, disk.retries_l1); // set retries for the input drive

 // avoid dsk_psecid() for types that do not support the function
 if (! input_sup.psecid)
    dsk_err = DSK_ERR_NOTIMPL;  // this will cause the values below to be used
 else
    // get the side information from first available sector header ID
    dsk_err = dsk_psecid(idrive, &dg, cyl, head, &result);

 if (dsk_err == DSK_ERR_OK)
    {
     *xhead = result.fmt_head;
     *xsecsize = result.fmt_secsize;
    }
 else // else if DSK_ERR_NOTIMPL or read error
    {
     // read sector ID call-back function if defined
     if (! xdg.rsi_cb || (xdg.rsi_cb && (*xdg.rsi_cb)
     (cyl, head, xhead, xsecsize) == 0))
         *xsecsize = cdg.dg_secsize;  // original geometry sector size     

     if (xdg.dg_side1as0)
        *xhead = 0;
     else
        *xhead = head + xdg.dg_sideoffs;
    }

 return DSK_ERR_OK;  // we always return OK here.
}

//==============================================================================
// Read a sector from the input drive using the appropriate function into
// the global buffer.
//
// If the sector concerned has already been read (buffered track) then
// nothing else needs to be done.  If there was an error in the track read
// buffering that contains the requested sector then 1 sector is read into
// the buffer.
//
//   pass: dsk_pcyl_t cyl               physical drive cylinder number
//         dsk_pcyl_t xcyl              ID cylinder value
//         dsk_phead_t head             physical drive side
//         dsk_phead_t xhead            ID side value
//         int lsect                    logical sector number
// return: dsk_err_t                    DSK_ERR_OK if successful
//==============================================================================
static dsk_err_t read_sector_x (dsk_pcyl_t cyl, dsk_pcyl_t xcyl,
                                dsk_phead_t head, dsk_phead_t xhead,
                                int lsect)
{
 DSK_FORMAT format[256];
 
 uint8_t *p;
 int psect;
 dsk_err_t dsk_err = DSK_ERR_OK;

 // set data rate and MFM/FM mode for input
 dg.dg_datarate = xdg.dg_idatarate;
 dg.dg_fm = xdg.dg_ifm;

 // set the 'format' structure for this track
 if (set_format_struct(cyl, head, xhead, format) == -1)
    return DSK_ERR_UNKNOWN;

 // read in one buffered track, if this was recorded as bad during a
 // previous track read then skip trying to buffer for the remainder of the
 // same error prone track and do it using sector reads instead
 if (buffered_cylinder != -2)
    {
     // read in the track if we don't already have it buffered
     if (buffered_cylinder != cyl || buffered_head != head)
        dsk_err = read_buffered_track(cyl, xcyl, head, xhead);

     // if the track was read in ok we are done!
     if (dsk_err == DSK_ERR_OK)
        return dsk_err;
    }

 // read 1 sector into the global buffer,  we get to here if a previous
 // buffer read error occured

 // get the skewed physical sector number
 psect = skew_table[lsect] + dg.dg_secbase;
 
 // calculate the buffer location where the sector data will be placed
 p = buf + dg.dg_secsize * (psect - dg.dg_secbase);

 // skip sector read call-back function if defined
 if (xdg.ssr_cb && ((*xdg.ssr_cb)(p, cyl, head, psect)))
    return DSK_ERR_OK;    

 // avoid dsk_xread() for types that do not support the function
 if (! input_sup.xread)
    dsk_err = DSK_ERR_NOTIMPL;
 else
    dsk_err = dsk_xread(idrive, &dg, p, cyl, head, xcyl, xhead, psect,
    dg.dg_secsize, NULL);
 
 if (dsk_err == DSK_ERR_NOTIMPL)
    dsk_err = dsk_pread(idrive, &dg, p, cyl, head, psect);
 
 return dsk_err;
}

//==============================================================================
// Read a sector from the input drive.
//
// Uses side checking and first level automatic retrying on read errors
// after the side information of sector IDs is found.
//
//   pass: dsk_pcyl_t cyl               cylinder number
//         dsk_phead_t head             physical side of disk
//         dsk_phead_t xhead            side ID value
//         int lsect                    logical sector number
// return: dsk_err_t                    DSK_ERR_OK if successful
//==============================================================================
static dsk_err_t read_sector (dsk_pcyl_t cyl, dsk_phead_t head,
                              dsk_phead_t xhead, int lsect)
{
 dsk_err_t dsk_err = DSK_ERR_OK;
 int count;
 int tryx;
 
 count = disk.retries_l2;  // this is level 2 (up from LibDsk retries)
 tryx = 0;

 // set data rate and MFM/FM mode for input
 dg.dg_datarate = xdg.dg_idatarate;
 dg.dg_fm = xdg.dg_ifm;
 
 // seems LibDsk DSK and RAW drivers do not support the --iside option and
 // only works for floppy access?  Provide a work around if setting --iside
 // causes an error using the normal method.
 if (disk.iside_not_support && disk.iside != -1)
    {
     head = disk.iside;
     xhead = disk.iside;  // not a perfect solution
    }

 for (;;)
    {
     tryx++;

     for (;;)
        {
         // read the sector into the global buffer
         dsk_err = read_sector_x(cyl, cyl, head, xhead, lsect);

         // some drives error out as it seems there is not enough delay
         // after the head is homed so for the first read error we will do
         // an unreported retry. (i.e Tandon TM848-02 8") 
         if (! disk.first_read)
            break;
         disk.first_read = 0;
         if (dsk_err == DSK_ERR_OK)
            break;

         // don't use close_reopen_pc_floppy_input_drive() here! instead just
         // use a delay or the head will try to home constantly.
         if (disk.verbose > 1)
            printf("read_sector(): sleep_ms(3000)!\n");
         sleep_ms(3000);
        }

     if (dsk_err == DSK_ERR_OK)
        return dsk_err;

     // if logging all read errors (good for drive diagnostics)
     if (disk.log == 2)
        log_read_error(dsk_err, tryx, cyl, head, lsect);

     sect_retry_count++;
     sect_retries_tot++;

     if (--count == 0)
        {
         printf("\n"APPNAME": read_sector() - %s\n", dsk_strerror(dsk_err));
         // if logging only errors when the retry count expires
         if (disk.log == 1)
            log_read_error(dsk_err, tryx, cyl, head, lsect);
         return dsk_err;
        }
    }    
}

//==============================================================================
// Read a sector from the input drive using recovery methods on errors.
//
// In interactive mode (--unattended=off) There is a choice of 6 options
// the operator may select when an error occurs:
//
// 1. reTry - Will try another read but does not seek to track 0 first.
// 2. Retry - Will seek to cylinder 0 and read a sector ID before retrying.
//    If the cylinder is already on 0 then cylinder 9 is used instead.
// 3. Ignore - An error is recorded and exits with no error.  The data is
//    likely to be corrupt.
// 4. Skip - Same as Ignore but for all remaining errors on current disk.
// 5. Abort - An error is recorded and exits with a disk error abort value.
// 6. Clean - Clean the drive heads using a special cleaning disk.
// 7. Mode - Toggle between Unattended and Interactive mode.
//
//   pass: dsk_pcyl_t cyl               cylinder number
//         dsk_phead_t head             physical side of disk
//         dsk_phead_t xhead            side ID value
//         int lsect                    logical sector number
// return: dsk_err_t                    DSK_ERR_OK if successful
//==============================================================================
static dsk_err_t read_sector_retry (dsk_pcyl_t cyl, dsk_phead_t head,
                                    dsk_phead_t xhead, int lsect)
{
 dsk_err_t dsk_err = DSK_ERR_OK;
 DSK_FORMAT sector_id;

 char user[100];
 sect_retry_count = 0;
 auto_retry_sector = 0;  // reset for the next sector
 
 for (;;)
    {
     // allow the operator to pause, resume, abort, clean and switch between
     // unattended and interactive modes
     if (pause_menu(0, cyl) == DSK_ERR_ABORT)
        {
         sect_retry_count = -2;
         return DSK_ERR_ABORT;
        }
     
     dsk_err = DSK_ERR_OK;
     
     if (disk.verbose)
        {
         printf("\rCylinder: %02d/%02d Head: %d/%d"
                " Sector: (log) %03d/%03d, (phy) %03d/%03d",
                cyl, dg.dg_cylinders - 1, head, dg.dg_heads - 1,
                lsect, dg.dg_sectors - 1,
                skew_table[lsect] + dg.dg_secbase,
                dg.dg_sectors + dg.dg_secbase - 1);
         fflush(stdout);
        }

     dsk_err = read_sector(cyl, head, xhead, lsect);

     if (disk.verbose)
        {
         if (dsk_err != DSK_ERR_OK)
            printf("\n");
        }    

     if (dsk_err == DSK_ERR_OK)
        return dsk_err;

     if (disk.unattended)
        {
         // If using the un-attended mode then errors are handled using a
         // pre-programmed series of actions.
         //
         // The 'auto_retry_sector' counts the number of retries for a given
         // sector and is reset to 0 for each new sector.  When this reaches
         // the maximum allowed value the 'auto_retry_abort' counter is
         // incremented and if that reaches the maximum allowed value then
         // the read process is aborted.
         //
         // Each time an error occurs a check will be made to see if a retry
         // using a seek track 0 should be initiated first or simply just
         // retry reading the same sector without any track 0 seek.  Once
         // the maximum number of track 0 seeks have been used up only
         // retries without seeking T0 are issued. (This is to reduce wear
         // on disks)
         
         if (auto_cyl_last != cyl || auto_head_last != head)
            {
             auto_move_head = 1;
             auto_cyl_last = cyl;
             auto_head_last = head;
             auto_retry_sector = 0;
            } 
         
         // once the retries are used up for the current sector read
         // increase the abort counter and if the maximum value is reached
         // abort.

         if (++auto_retry_sector > disk.unattended_retry_sector_max)
            {
             if (++auto_retry_abort > disk.unattended_retry_abort_max)
                user[0] = 'A';
             else
                user[0] = 'I';
            }
         else   
            {
             // decide if the head will be moved first before retrying

             if (auto_move_head &&
                 auto_seeked_count < disk.unattended_seeked_max)
                {
                 user[0] = 'R';
                 auto_move_head = 0;
                 auto_seeked_count++;
                }
             else
                user[0] = 'T';
            } 

         printf("Unattended mode:\n");
         printf("re[T]ry (no home) [R]etry (seek home) [I]gnore [A]bort"
                " ---> '%c'\n", user[0]);
         if (user[0] != 'I' && user[0] != 'A')
            printf("Retries for disk: %d/%d  Retries for sector:"
                   " %d/%d  Track #0 retry seeks: %d/%d\n\n",
            auto_retry_abort,  disk.unattended_retry_abort_max,
            auto_retry_sector, disk.unattended_retry_sector_max,
            auto_seeked_count, disk.unattended_seeked_max);
         else   
            printf("Error in read operation, ignoring last sector and"
                   " recording as an error!\n\n");
        }
     else
        {
         if (skip_all_errors)
            user[0] = 'I';
         else
            {
             // Operator to decide what to do. This also allows the unattended
             // or Interactive mode to be set or the drive heads to be cleaned.
             user[0] = 0;
             while ((toupper(user[0]) != 'T') && (toupper(user[0]) != 'R') &&
                    (toupper(user[0]) != 'I') && (toupper(user[0]) != 'A') &&
                    (toupper(user[0]) != 'S'))
                {
                 printf("INTERACTIVE ERROR MENU\n");
                 printf("----------------------\n");
                 printf("re[T]ry (no home) [R]etry (home) [I]gnore [S]kip"
                        " [A]bort [C]lean [M]ode ?: ");
                 fflush(stdout);
                 if (fgets(user, sizeof(user)-2, stdin))
                    {} // ignore result
                 switch (toupper(user[0]))
                    {
                     case 'C' : // clean drive heads with special disk
                        disk_clean(0, cyl);
                        printf("\n");
                        break;
                     case 'M' : // switch to unattended or interactive mode                 
                        disk.unattended = ! disk.unattended;
                        if (disk.unattended)
                           printf("Unattended mode set!\n");
                        else
                           printf("Interactive mode set!\n");
                        printf("\n");
                        break;
                     default :
                        printf("\n");
                        break;
                    }
                }
            }
        }

     switch (toupper(user[0]))
        {
         case 'T' : // retry but don't move head
            break;
         case 'R' : // retry after moving the head first
            // Need to do the following if drive door was opened
            // when trying to read on some drives.
            home_and_reset_input_drive_and_settings(&sector_id);

            // if we were on track 0 we move the head to T1 first
            // then back again.
            if (cyl != 0)
               break;
            dsk_psecid(idrive, &dg, 1, 0, &sector_id);
            dsk_psecid(idrive, &dg, 0, 0, &sector_id);
            break;
         case 'S' : // All future errors for this disk will be ignored.
            // First time we fall through to case 'I'.
            skip_all_errors = 1;
         case 'I' : // mark as an error but we return an OK to keep going
            sect_errors_tot++;
            sect_retry_count = -1;
                    
            // Need to do the following if drive door was opened
            // when trying to read on some drives.
            home_and_reset_input_drive_and_settings(&sector_id);
            return DSK_ERR_OK;    
         case 'A' : // abort the copy with an error
            sect_errors_tot++;
            sect_retry_count = -1;
            return DSK_ERR_ABORT; 
        }
    }
}

//==============================================================================
// Initialise the info file values.
//
// Initialises some variables.  The entire info file is created in the
// create_info_file() function.
//
//   pass: void
// return: void
//==============================================================================
static void init_info_file (void)
{
 info.cyl_last = -1;
 info.head_last = -1;
 info.pos_trk = 0;
 info.buf[0] = -1;
}

//==============================================================================
// Create info file
//
//   pass: void
// return: int                          0 if no error else -1
//==============================================================================
static int create_info_file (void)
{
 char *month_names[] =
 {
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
 };

 sup_args_t drive_args[] =
 {
  {"disk image or unknown",   0}, 
  {"400kb 5.25\"",          'd'},
  {"1.2 Mb 5.25\"",         'h'},
  {"800kb 3.5 or 5.25\"",   'D'},
  {"1.44 Mb 3.5\"",         'H'},  
  {"2.88 Mb",               'E'},
  {"",                       -1}
 };

 typedef struct tm tm_t;

 time_t result;
 tm_t resultp;

 char temp_str[100];
 char date_time[100];
 
 int i;
 int x;
 int retries;

 int secbase_last = -1;
 int sectors_last = -1;
 int secsize_last = -1;

 char *res;
 char inpf[1000];
 char outf[1000];
 char md5[33];

 char info_file[1000];
 
 // check if output type is a disk or other non-image and skip the info file
 // if it is.
 if (! disk.info_file || string_search(no_info_file, disk.otype) != -1)
    return 0;

 // create the info file
 snprintf(info_file, sizeof(info_file), "%s.info", ofile_name);
 info_file[sizeof(info_file)-1] = 0;

 infof = fopen(info_file, "w");
 if (infof == NULL)
    {
     printf(APPNAME": unable to create info file: %s\n", info_file);
     return -1;
    }

 // create a date and time stamp
 time(&result); 
#ifdef WIN32
 memcpy(&resultp, localtime(&result), sizeof(resultp));
#else
 localtime_r(&result, &resultp);
#endif 
 if (resultp.tm_sec > 59)               // 2 second leap-seconds ignore
    resultp.tm_sec = 59;

 sprintf(date_time, "%d %s %d  %02d:%02d:%02d", resultp.tm_mday,
 month_names[resultp.tm_mon],
 resultp.tm_year+1900, resultp.tm_hour, resultp.tm_min, resultp.tm_sec);
 
 // compute an MD5 for the image created
 create_md5(ofile_name, md5);
 
 // extract the file names from the paths
 file_name_part(disk.ifile, inpf);
 file_name_part(ofile_name, outf);

 fprintf(infof, "DISK/IMAGE INFORMATION\n");
 fprintf(infof, "----------------------\n");
 fprintf(infof, "File output        %s\n", outf);
 fprintf(infof, "File output MD5    %s\n", md5);
 fprintf(infof, "Disk name          %s\n", xdg.dg_format_name);
 fprintf(infof, "Disk name desc     %s\n", xdg.dg_format_desc);
 fprintf(infof, "Archived by        %s\n", disk.signature);
 fprintf(infof, "Creation date      %s\n\n", date_time);
 
 fprintf(infof, "DISK DESCRIPTION\n");
 fprintf(infof, "----------------\n");
 i = 0;
 for (;;)
    {
     if (description[i][0] && strcmp(".\n", description[i]) != 0)
        fprintf(infof, "%s", description[i++]);
     else
        break;
    }
 fprintf(infof, "\n");   

 fprintf(infof, "PROGRAM PARAMETERS\n");
 fprintf(infof, "------------------\n");          
 fprintf(infof, "Image program      "TITLESTRING"\n");
 fprintf(infof, "Detection req      %s\n", detect_disk_args[disk.detect]);
 fprintf(infof, "Disk format req    %s\n", disk.format);
 fprintf(infof, "File input         %s\n", inpf);
 fprintf(infof, "Input type         %s\n", disk.itype);
 fprintf(infof, "Output type        %s\n", disk.otype);
 fprintf(infof, "Input dstep        %s\n", noyes[disk.idstep_used]);
 fprintf(infof, "Output dstep       %s\n", noyes[disk.odstep_used]);
 fprintf(infof, "Input side         %s\n", sides[disk.iside+1]);
 fprintf(infof, "Output side        %s\n", sides[disk.oside+1]);
 fprintf(infof, "Start cylinder     %d\n", cyl_start);
 fprintf(infof, "Finish cylinder    %d\n", cyl_finish);
 fprintf(infof, "Start track (log)  %d\n", trk_start);
 fprintf(infof, "Finish track (log) %d\n", trk_finish);
 fprintf(infof, "Force side         %s\n", forceside_values[disk.forceside]);
 fprintf(infof, "Input comp         %s\n", disk.incomp);
 fprintf(infof, "Output comp        %s\n", disk.outcomp);
 fprintf(infof, "Retries lev1 (max) %d\n", disk.retries_l1);
 fprintf(infof, "Retries lev2 (max) %d\n", disk.retries_l2);
 fprintf(infof, "Logging level      %d\n", disk.log);
 fprintf(infof, "Input data rate    %s\n", datarates_str[xdg.dg_idatarate]);
 fprintf(infof, "Output data rate   %s\n", datarates_str[xdg.dg_odatarate]);

 fprintf(infof, "\n");
 fprintf(infof, "UNATTENDED MODE SETTINGS\n");
 fprintf(infof, "------------------------\n"); 
 fprintf(infof, "Used unattended    %s\n", noyes[disk.unattended]);
 fprintf(infof, "Sector ret/abort   %d\n", disk.unattended_retry_abort_max); 
 fprintf(infof, "Retries/sector max %d\n", disk.unattended_retry_sector_max); 
 fprintf(infof, "Error seeks max    %d\n", disk.unattended_seeked_max); 

 fprintf(infof, "\n");
 fprintf(infof, "PHYSICAL ATTRIBUTES\n");
 fprintf(infof, "-------------------\n");
 if ((x = string_struct_search_i(drive_args, disk.idrive_type)) == -1)
    x = 0;
 fprintf(infof, "Input drive type   %s\n", drive_args[x].name);
 if ((x = string_struct_search_i(drive_args, disk.odrive_type)) == -1)
    x = 0;
 fprintf(infof, "Output drive type  %s\n", drive_args[x].name);

 fprintf(infof, "Sidedness          %s\n", sidedness[dg.dg_sidedness]);
 fprintf(infof, "Data rate          %s\n", datarates_str[xdg.dg_odatarate]);
 fprintf(infof, "Cylinders          %d\n", dg.dg_cylinders);
 fprintf(infof, "Heads              %d\n", dg.dg_heads);

 fprintf(infof, "Sectors/track      ");
 if (! xdg.cif_cb || (xdg.cif_cb && (*xdg.cif_cb)(0) == 0))
    fprintf(infof, "%d\n", dg.dg_sectors); 

 fprintf(infof, "Sector size        ");
 if (! xdg.cif_cb || (xdg.cif_cb && (*xdg.cif_cb)(1) == 0))
    fprintf(infof, "%d\n", dg.dg_secsize);

 fprintf(infof, "Sector base #1     %d\n", xdg.dg_secbase1s);
 fprintf(infof, "Sector base #2     %d\n", xdg.dg_secbase2s);
 fprintf(infof, "Read skew          %d\n", xdg.dg_skew_val);
 fprintf(infof, "Read skew offset   %d\n", xdg.dg_skew_ofs);

 fprintf(infof, "\n");
 fprintf(infof, "DISK COPY RESULTS\n");     
 fprintf(infof, "-----------------\n");
 fprintf(infof, "Sector errors      %d\n", sect_errors_tot);
 fprintf(infof, "Sector retries     %d\n", sect_retries_tot);     

 fprintf(infof, "\n");
 fprintf(infof, "SECTOR STATUS MAP\n");
 fprintf(infof, "-----------------\n");

 fprintf(infof,
"A sector position containing a '.' indicates the sector read correctly first\n"
"time. An 'X' indicates the sector was not read (terminated early). An 'ERR'\n"
"indicates the sector had a read error and a value from '1-999' indicates the\n"
"number of level 2 retries that were needed to successfully read the sector."
"\n\n");

 i = 0;
 while (info.buf[i] != -1)
    {
     if (info.buf[i] != secbase_last || info.buf[i+1] != sectors_last ||
         info.buf[i+2] != secsize_last)
        {
         secbase_last = info.buf[i];
         sectors_last = info.buf[i+1];
         secsize_last = info.buf[i+2];
         sprintf(temp_str, "(%dx%d)", sectors_last, secsize_last);
         fprintf(infof, "Cylinder  Head%11s  ", temp_str);
         for (x = 0; x < sectors_last; x++)
            fprintf(infof, "%03d ", secbase_last + x);
         fprintf(infof, "\n");
        } 
     i += 3;
     fprintf(infof, "%4d       %d               ", info.buf[i], info.buf[i+1]);
     i += 2;
     for (x = 0; x < sectors_last; x++)
        {
         switch (info.buf[i])
            {
             case 0 : // sector is marked as good
                fprintf(infof, " .  ");
                i++;    
                break;
             case 1000 : // sector is marked as bad
                fprintf(infof, "ERR ");
                i++;   
                break;
             case 1001 : // sector was not read (aborted)
                fprintf(infof, " X  ");
                i++;
                break;
             default : // sector is good but required some retries
                retries = info.buf[i];
                if (retries > 255)
                   retries = 255;
                fprintf(infof, "%3d ", retries);
                i++;    
            } 
        }
     fprintf(infof, "\n");                
    }

 // append or close the error log file if one was created
 if (errorf)
    {
     if (disk.append_error)
        {
         rewind(errorf);
         fprintf(infof, "\n");
         while ((res = fgets(temp_str, sizeof(temp_str)-1, errorf)))
            fprintf(infof, "%s", temp_str);        
         fclose(errorf);
         errorf = NULL;
         remove(error_file);
        }
     else
        {
         fclose(errorf);
         errorf = NULL;
        }
    }  

 fclose(infof);
 infof = NULL;
 return 0;
}

//==============================================================================
// Place a sector read result into the info file buffer.
//
// The status of sector reads is placed in the table in the physical sector
// locations.  All sectors for each new track are filled with -1 to indicate
// a sector that has not been read and a -1 end marker inserted for the next
// track to indicate end of info.
//
//   pass: dsk_pcyl_t cyl               cylinder number
//         dsk_phead_t head             physical side of disk
//         int lsect                    logical sector number
// return: void
//==============================================================================
static void info_file_entry (dsk_pcyl_t cyl, dsk_phead_t head, int lsect)
{
 int i;
 
 if ((info.pos_trk + 10) > INFO_SIZE)
    return;

 if (cyl != info.cyl_last || head != info.head_last)
    {
     info.buf[info.pos_trk++] = dg.dg_secbase;
     info.buf[info.pos_trk++] = dg.dg_sectors;
     info.buf[info.pos_trk++] = dg.dg_secsize;
     info.buf[info.pos_trk++] = cyl;
     info.buf[info.pos_trk++] = head;

     // sectors for this track will be inserted here
     info.pos_sec = info.pos_trk;

     // the next track index
     info.pos_trk += dg.dg_sectors;
     
     // set all sectors in this track with 'not read' status values
     for (i = 0; i < dg.dg_sectors; i++)
        info.buf[info.pos_sec + 1] = -2;
        
     // mark the next track as not used (end marker)
     info.buf[info.pos_trk] = -1;
     
     info.cyl_last = cyl;
     info.head_last = head;
    }

 // offset index for the physical sector to be inserted
 i = info.pos_sec + skew_table[lsect];

 // set the status of the last sector read 
 if (sect_retry_count == -1) // sector read error?
    info.buf[i] = 1000;  
 else
     if (sect_retry_count == -2) // unread sector?
        info.buf[i] = 1001;
     else  // else set the retry count
        info.buf[i] = sect_retry_count;

#if 0
 printf(" - info.pos_trk=%d, info.pos_sec=%d i=%d\n",
        info.pos_trk, info.pos_sec, i);
#endif
}

//==============================================================================
// Close all open libdsk files.
//
//   pass: void
// return: void
//==============================================================================
void close_files (void)
{
 dsk_err_t dsk_err = DSK_ERR_OK;

 if (idrive)
    {
     if (dsk_close(&idrive) != DSK_ERR_OK)
        printf(APPNAME": dsk_close(&idrive) - %s\n", dsk_strerror(dsk_err));
     idrive = NULL;
    } 

 if (odrive)
    {
     if (dsk_close(&odrive) != DSK_ERR_OK)
        printf(APPNAME": dsk_close(&odrive) - %s\n", dsk_strerror(dsk_err));
     odrive = NULL;
    } 
}

//==============================================================================
// Set the data rate and stepping for input and output floppy drives
// supported by the itype and otype drivers.
//
// The auto rate and auto double stepping sections of this function is
// really only intended for 40T 5.25" disks in a 5.25" 80T drive.
//
// This function overrides a datarate(s) if a --datarate=rate,
// --datarateip=rate or --datarateop=rate option was specified, normally the
// datarate is initiallay set in the 'format' model and may be further set
// later by a detection module.  The --datarate* options if specified always
// determines the datarate.
//
// Note: If a PC has an 80T DD drive fitted (very unusual) then a disk
// definition for it using RATE_SD should be used, the autorate won't act on
// it here because it should be seen as non 'h' drive type unless the PC's
// BIOS has been set incorrectly.  Such drives should probably be set to
// 5.25" 360kb if no setting for 720k is provided.
//
//   pass: void
// return: dsk_err_t                    DSK_ERR_OK if successful
//==============================================================================
static dsk_err_t set_rate_step (void)
{
 dsk_err_t dsk_err = DSK_ERR_OK;

 // auto set the data rate and stepping for the input drive
 if (idrive && disk.iautorate)
    {
     // if this is a (80T 5.25" HD or 77T 8") drive and less than 77
     // cylinders (70) then assume it to be a DD 40T 400K disk
     if (disk.idrive_type == 'h' && disk.idstep != 0 && dg.dg_cylinders < 70)
        {
         xdg.dg_idatarate = RATE_DD;
         disk.idstep_used = 1;
        }
    }

 // auto set the data rate and stepping for the output drive
 if (odrive && disk.oautorate)
    {
     // if this is a (80T 5.25" HD or 77T 8") drive and less than 77
     // cylinders (70) then assume it to be a DD 40T 400K disk
     if (disk.odrive_type == 'h' && disk.odstep != 0 && dg.dg_cylinders < 70)
        {
         xdg.dg_odatarate = RATE_DD;
         disk.odstep_used = 1;
        }
    }

 // override the input datarate if it was specified
 if (dg_opts.idatarate != -1)
    xdg.dg_idatarate = dg_opts.idatarate;

 // override the output datarate if it was specified
 if (dg_opts.odatarate != -1)
    xdg.dg_odatarate = dg_opts.odatarate;    

 // override the double stepping for input if it was specified
 if (idrive && dsk_err == DSK_ERR_OK && disk.idstep == 1)
    disk.idstep_used = 1;

 // override the double stepping for output if it was specified
 if (odrive && dsk_err == DSK_ERR_OK && disk.odstep == 1)
    disk.odstep_used = 1;

 // if supported by the LibDsk driver disable the AUTORATE detection
 if (idrive)
    dsk_err = dsk_set_option(idrive, "AUTORATE", disk.iautorate);
 if (odrive)
    dsk_err = dsk_set_option(odrive, "AUTORATE", disk.oautorate);

 // enable double stepping if requested
 if (disk.idstep_used)
    dsk_err = dsk_set_option(idrive, "DOUBLESTEP", 1);
 if (disk.odstep_used)
    dsk_err = dsk_set_option(odrive, "DOUBLESTEP", 1);

 return DSK_ERR_OK;
}

//==============================================================================
// Convert macro file names and types to the format expected by the host
// allowing the ubeedisk.ini file to be portable between host platforms.
//
// Converts driver type 'fd' to the host floppy driver 'ntwdm' on Windows and
// 'floppy' on Unices.
// 
// Converts drives 'A:' - 'Z:' on Unices to devices /dev/fdx
//
// Set LibDsk driver types based on file names if the user has not specified
// the type.
//
//   pass: char *xfile
//         char *xtype
// return: void
//==============================================================================
#define report_xtype_values(id) \
if (disk.verbose > 1) \
   printf("set_xtype_xfile() (%d) xfile=%s, xtype=%s\n", \
   id, xfile, xtype);

static void set_xtype_xfile (char *xfile, char *xtype)
{
 char *p;
 int i;

 ext_type_t ext_type[] =
 {
  {".dsk",      "dsk"},
  {".edsk",    "edsk"},
  {".imd",      "imd"},  
  {".qm",        "qm"},
  {".tel",     "tele"},
  {".cfi",      "cfi"},
  {".nw",  "nanowasp"},
  {".raw",      "raw"},
  {".img",      "raw"},
  {".ss40",     "raw"},
  {".ss80",     "raw"},
  {".ds40",     "raw"},
  {".ds80",     "raw"},
  {".ds82",     "raw"},
  {".ds84",     "raw"},
  {".ds8x",     "raw"},
  {".ds40s",    "raw"},  
  {".ss40s",    "raw"},
  {"",             ""}
 };

 report_xtype_values(1);

 // driver 'fd' may be used to represent the host floppy driver
 if (strcmp(xtype, "fd") == 0)
#ifdef WIN32
    strcpy(xtype, "ntwdm");
#else
    strcpy(xtype, "floppy");
#endif

 // drives 'A:' - 'Z:' may be used on Unices to represent /dev/fdx
#ifndef WIN32
 if ((strstr(xfile, ":") == xfile + 1) && (strlen(xfile) == 2) &&
 (toupper(xfile[0]) >= 'A') && (toupper(xfile[0]) <= 'Z'))
    sprintf(xfile, "/dev/fd%d", toupper(xfile[0]) - 'A');
#endif

 // do nothing if a type has been specified or no file name
 if (xtype[0] || ! xfile[0])
    {
     report_xtype_values(2);
     return;
    }

 // check if we are using a serial remote file, if so set remote type
 if (strstr(xfile, "serial:") == xfile)
    {
     strcpy(xtype, "remote");
     report_xtype_values(3);
     return;
    }

 // determine the type required from the file name    
#ifdef WIN32
 if ((strstr(xfile, ":") == xfile + 1) && (strlen(xfile) == 2) &&
 (toupper(xfile[0]) >= 'A') && (toupper(xfile[0]) <= 'B'))
    strcpy(xtype, "ntwdm");
#else
 if (strstr(xfile, "/dev/fd") == xfile)
    strcpy(xtype, "floppy");
#endif
 else
    {
     for (i = 0; ext_type[i].ext[0]; i++)
        {
         if ((p = strstr(xfile, ext_type[i].ext)) &&
            (*(p + strlen(ext_type[i].ext)) == 0))
            {
             strcpy(xtype, ext_type[i].type);
             break;
            }
        }
    }

 report_xtype_values(4);
}

//==============================================================================
// Override values.
//
// After setting the geometry values elsewhere (from a 'format') this
// function allows overriding values specified on the command line.  It also
// allows 'formats' to be specified in 'ubeedisk.ini' which first sets a
// default format.
//
// This also makes a copy of the current geometry values (cdg) as some
// values may be set on the fly and need the original values.
//
//   pass: void
// return: dsk_err_t                    DSK_ERR_OK if successful
//==============================================================================
dsk_err_t override_values (void)
{
 int i;
 int x;
 
 dsk_err_t dsk_err = DSK_ERR_OK;
 dsk_err_t temp_dsk_err;

 // set the disk sidedness format
 if (dg_opts.sidedness != -1)
    dg.dg_sidedness = dg_opts.sidedness;

 // set the number of cylinders
 if (dg_opts.cylinders != -1)
    dg.dg_cylinders = dg_opts.cylinders;

 // set the mumber of heads
 if (dg_opts.heads != -1)
    dg.dg_heads = dg_opts.heads;

 // set the number of sectors per track
 if (dg_opts.sectors != -1)
    dg.dg_sectors = dg_opts.sectors;

 // set the sector base 
 if (dg_opts.secbase != -1)
    {
     dg.dg_secbase = dg_opts.secbase;
     xdg.dg_secbase1s = dg_opts.secbase;
    } 

 // set the bytes per sector 
 if (dg_opts.secsize)
    dg.dg_secsize = dg_opts.secsize;

 // set the read/write gap
 if (dg_opts.rwgap != -1)
    dg.dg_rwgap = dg_opts.rwgap;

 // set the format gap
 if (dg_opts.fmtgap != -1)
    dg.dg_fmtgap = dg_opts.fmtgap;

 // set no skip of deleted data
 if (dg_opts.noskip != -1)
    dg.dg_noskip = dg_opts.noskip;

 // set no multi read/write
 if (dg_opts.nomulti != -1)
    dg.dg_nomulti = dg_opts.nomulti;

 // set the input MFM/FM mode
 if (dg_opts.ifm != -1)
    xdg.dg_ifm = dg_opts.ifm;

 // set the outut MFM/FM mode
 if (dg_opts.ofm != -1)
    xdg.dg_ofm = dg_opts.ofm;

 // set sideoffs
 if (dg_opts.sideoffs != -1)
    xdg.dg_sideoffs = dg_opts.sideoffs;

 // set side1as0
 if (dg_opts.side1as0 != -1)
    xdg.dg_side1as0 = dg_opts.side1as0;

 // set secbase2s
 if (dg_opts.secbase2s != -1)
    xdg.dg_secbase2s = dg_opts.secbase2s;

 // set secbase2c
 if (dg_opts.secbase2c != -1)
    xdg.dg_secbase2c = dg_opts.secbase2c;

 // sector skew value
 if (dg_opts.skew_val != -1)
    xdg.dg_skew_val = dg_opts.skew_val;

 // offset to apply to sector base
 if (dg_opts.skew_ofs != -1)
    xdg.dg_skew_ofs = dg_opts.skew_ofs;

 // set special flags
 if (dg_opts.special != -1)
    xdg.dg_special = dg_opts.special;

 // set format name and description that is defined in a 'format'
 if (dg_opts.format_name[0] != 0)
    {
     strcpy(xdg.dg_format_name, dg_opts.format_name);
     strcpy(xdg.dg_format_desc, dg_opts.format_desc);
    } 

 // set sides for input
 if (idrive && disk.iside != -1)
    {
     temp_dsk_err = dsk_set_option(idrive, "HEAD", disk.iside);
     if (temp_dsk_err != DSK_ERR_OK)
        disk.iside_not_support = 1;
    }

 // set sides for output
 if (odrive && disk.oside != -1 && dsk_err == DSK_ERR_OK)
    {
     temp_dsk_err = dsk_set_option(odrive, "HEAD", disk.oside);
     if (temp_dsk_err != DSK_ERR_OK)
        disk.oside_not_support = 1;
    }

 // set GAP and SYNC values for devices that support it, we ignore any
 // errors reported for these
 for (i = 0; i < 4; i++)
    {
     x = i * 2;
     if (disk.gap_set[x] != -1)
        dsk_set_option(odrive, "GAPSET", (i << 14) + disk.gap_set[x]);

     x++;
     if (disk.gap_set[x] != -1)
        dsk_set_option(odrive, "SYNCSET", (i << 14) + disk.gap_set[x]);
    }        

 // enable/disable read/write caching in device hardware if supported, we
 // ignore any errors reported for these
 if (disk.cacher != -1)
    dsk_set_option(idrive, "RBUFFER", disk.cacher);

 if (disk.cachew != -1)
    dsk_set_option(odrive, "WBUFFER", disk.cachew);

 // set disk erase value (used in a format only) in device hardware if
 // supported, we ignore any errors reported for this
 if (disk.erase != -1)
    dsk_set_option(odrive, "ERASE", disk.erase);

 // if --pskew options were used then set the skew pointers
 if (disk.pskew0_opt)
    xdg.dg_pskew0 = disk.pskew0;

 if (disk.pskew1_opt)
    xdg.dg_pskew1 = disk.pskew1;
    
 // set the data rate and stepping for the input/output drives
 if (dsk_err == DSK_ERR_OK)
    dsk_err = set_rate_step();

 // not many retries at the first level should be needed if the 'remote'
 // driver is selected as native hardware should be more reliable and having
 // a high value simply slows down the process too much if there is a bad
 // sector, when using the LibDsk driver we tend to see a lot of soft errors
 // so the value needs to be higher.
 if (disk.retries_l1 == -1)
    {
     if (strcmp("remote", disk.itype) == 0)
        disk.retries_l1 = 3;
     else
        {
         if (disk.disk == UBEEDISK_SCAN || disk.disk == UBEEDISK_SPEED)
            disk.retries_l1 = 5;
         else   
            disk.retries_l1 = 30;
        }
    }

 // set ignore all errors when in Interactive mode.
 if (disk.ignore_errors == 1)
    skip_all_errors = 1;
 else   
    skip_all_errors = 0;
    
 // this will be the first read of a sector if copying a disk
 disk.first_read = 1;

 // make a copy of the current geometry values
 memcpy(&cdg, &dg, sizeof(cdg));

 if (dsk_err != DSK_ERR_OK)
    {
     printf(APPNAME": override_values() - %s\n", dsk_strerror(dsk_err));
     return dsk_err;
    }

 return DSK_ERR_OK;
}

//==============================================================================
// Determine what interface support there is for an already open input or
// output drive.  This is needed by remote types FDS.COM and Floppyio to
// determine if full support for dsk_psecid(), etc is implemented.  It may
// be used for other things in the future.
//
// FDS.COM does not currently fully implement the dsk_psecid() function and
// the head value returned is what is passed to it.  Floppyio does fully
// implement the function.
//
//   pass: DSK_PDRIVER xdrive           input or output drive
//         char *xtype                  input or output type
//         sup_t *xsup                  input or output support structure
// return: void
//==============================================================================
static void interface_support (DSK_PDRIVER xdrive, char *xtype, sup_t *xsup)
{
 char *p[1];
 int i;

 // clear all the support values
 memset(xsup, 0, sizeof(sup_t));

 // if not the 'remote' type
 if (strcmp("remote", xtype))
    {
     // if not the 'raw' type
     if (strcmp("raw", xtype))
        {
         xsup->psecid = 1;
         xsup->ptrackids = 1;
         xsup->xread = 1;
         xsup->xwrite = 1;
        }

     xsup->pformat = 1;
     strcpy(xsup->version, "n/a");
     strcpy(xsup->remote_interface, "n/a");
     return;
    }

 // FDS.COM and Floppyio partially support xread and xwrite 
 xsup->xread = 1;
 xsup->xwrite = 1;

 for (i = 0; ;i++)
    {
     dsk_option_enum(xdrive, i, p);
     if (*p && strstr(*p, "Interface:") == *p)
        {
         strcpy(xsup->remote_interface, *p + 11);
         if (! strcmp(xsup->remote_interface, "Floppyio"))
            {
             xsup->psecid = 1;
             xsup->ptrackids = 1;
             xsup->pformat = 1;

             // if required the next enumeration should hold the version
             i++;
             dsk_option_enum(xdrive, i, p);
             if (*p)
                strcpy(xsup->version, *p + 9);
            }
         return;
        }
     else
        if (! *p)
           return;
    }
}

//==============================================================================
// Check input disk/image drive and disk type.
//
//   pass: void
// return: int                          0 if no error else -1
//==============================================================================
static int check_input_drive (void)
{
 if (! *disk.ifile)
    return 0;

 if (! *disk.itype)
    {
     printf(APPNAME": use the --itype option to specify the input type\n");
     return -1;
    }

 if ((string_search(is_not_var_track_size_type, disk.itype) != -1) &&
 (xdg.dg_attributes & ATTR_VAR_TRACK_SIZE))
    {
     printf("\n"APPNAME": the format has a variable track size and requires"
     " an input type that allows for it\n");
     return -1;
    }

 return 0;
}

//==============================================================================
// Check output disk/image drive and disk type.
//
//   pass: void
// return: int                          0 if no error else -1
//==============================================================================
static int check_output_drive (void)
{
 if (! *disk.ofile)
    return 0;

 if (! *disk.otype)
    {
     printf(APPNAME": use the --otype option to specify the output type\n");
     return -1;
    }

 if ((string_search(is_not_var_track_size_type, disk.otype) != -1) &&
 (xdg.dg_attributes & ATTR_VAR_TRACK_SIZE))
    {
     printf("\n"APPNAME": the format has a variable track size and requires"
     " an output type that allows for it\n");
     return -1;
    }

 return 0;
}

//==============================================================================
// Open input disk/image drive.
//
// Opens input drive to a default format or another if specified in
// disk.format.
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
static int open_input_drive (void)
{
 dsk_err_t dsk_err = DSK_ERR_OK;

 char *incomp;
 
 // open the input drive
 if (*disk.incomp)
    incomp = disk.incomp;
 else
    incomp = NULL;

 if (disk.verbose > 1)
    printf("open_input_drive()\n");
 
 reset_drive(disk.itype, disk.ifile);

 //printf("disk.ifile=%s disk.itype=%s\n", disk.ifile, disk.itype);

 dsk_err = dsk_open(&idrive, disk.ifile, disk.itype, incomp);

 if (dsk_err != DSK_ERR_OK)
    {
     printf(APPNAME": open_input_drive() - %s\n", dsk_strerror(dsk_err));
     return -1;
    }

 disk.idrive_type = get_drive_type(disk.itype, disk.ifile, idrive);
 if (disk.verbose > 1)
    printf("Input drive type: %c (char) %d (dec)\n",
           disk.idrive_type, disk.idrive_type);

 interface_support(idrive, disk.itype, &input_sup);

 if (disk.verbose > 1)
    {
     printf("Input interface name: %s\n", input_sup.remote_interface);
     printf("Input interface version: %s\n", input_sup.version);
    }

 return 0;
}

//==============================================================================
// Create output disk/image drive.
//
// Creates an output disk using the same geometry as the input drive
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
static int create_output_drive (void)
{
 dsk_err_t dsk_err = DSK_ERR_OK;

 char *outcomp;

 if (*disk.outcomp)
    outcomp = disk.outcomp;
 else
    outcomp = NULL;
 
 reset_drive(disk.otype, ofile_name);
 
 dsk_err = dsk_creat(&odrive, ofile_name, disk.otype, outcomp);

 if (dsk_err != DSK_ERR_OK)
    {
     printf(APPNAME": create_output_drive() - %s\n", dsk_strerror(dsk_err));
     return -1;
    }

 disk.odrive_type = get_drive_type(disk.otype, ofile_name, odrive);
 if (disk.verbose > 1)
    printf("Output drive type: %c (char) %d (dec)\n",
           disk.odrive_type, disk.odrive_type);

 // retry value of 1 only writes once
 dsk_set_retry(odrive, 10);

 interface_support(odrive, disk.otype, &output_sup);
 if (disk.verbose > 1)
    {
     printf("Output interface name: %s\n", output_sup.remote_interface);
     printf("Output interface version: %s\n", output_sup.version);
    }

 return 0;
}

//==============================================================================
// Modify the output file name using substitution.
//
// This allows some LibDsk geometry and other values to be inserted into the
// file name in a readable form.  A substitution is carried out when a '@'
// character is found in the file name specified.  The single character that
// follows then determines the substitution value.  Normal text characters
// may be used in and around substitutions.
//
// There are three special "command" characters that are used to determine
// if the substitution is to use upper or lower case characters and one for
// terminating a concatenated substitution sequence (internal use).
//
// This is very useful for use with --detect=dos and any non-raw disk types
// so that the actual format can be included in the file name.
//
// Substitution values
// -------------------
// @h sided SS/DS
// @H heads
// @d density SD/DD/HD
// @f recording format FM/MFM
// @c number of cylinders
// @t sectors per track
// @s sector size
// @m media descriptor byte (DOS disks only)
// @n name of media format (i.e. ds40, ds80, applix80, dos)
// @@ inserts a '@' character
//
// Substitution command values
// ---------------------------
// @u following substitutions will be in upper case
// @l following substitutions will be in lower case (default)
// @z terminates a concatenated substitution sequence (for internal use)
//
// Concatenated substitutions sequences
// ------------------------------------
// @0 ->               @h@d_@f_@cx@tx@s_@m@z     ('_@m' only if --detect=dos)
// @1 ->               @n_@h@d_@f_@cx@tx@s_@m@z  ('_@m' only if --detect=dos)
// @2 ->               @n_@h@d_@f_@cx@tx@s@z
// @3 ->               @n_@h@d_@cx@tx@s@z
// @4 ->               @n_@cx@tx@s@z
// @5 ->               @cx@tx@s@z
// @r ->               .fdd-@c-@H-@t-@s@z        (uBee512 RAW images)
//
// Examples for a 360k DOS disk and DSK image
// ------------------------------------------
// @1.dsk ->           dos_dsdd_mfm_40x9x512_(FD).dsk
// @2.dsk ->           dos_dsdd_mfm_40x9x512.dsk
// @3.dsk ->           dos_dsdd_40x9x512.dsk
// @4.dsk ->           dos_40x9x512.dsk
// @5.dsk ->           40x9x512.dsk
// blah_@1_blah.dsk -> blah_dos_dsdd_mfm_40x9x512_(FD)_blah.dsk
//
// Examples for a Microbee CIAB DSK image converted to RAW format
// --------------------------------------------------------------
// ubeedisk --format=ss80 --if=ciab.dsk --otype=raw --of=disk@r
// ubeedisk --format=ss80 --if=ciab.dsk --otype=raw --of=@n@r
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
static int output_filename_substitution (void)
{
 char subs_ofile_name[1000];
 char temp_str[100];
 char job[100];
 char *p = ofile_name;
 char *saved_p = NULL;
 int use_upper = 0;
 int l = 0;

 *subs_ofile_name = 0;
 *temp_str = 0;
 *job = 0;
 
 while (*p)
    {
     // if not doing a concatenated substitution sequence
     if (! *job)
        saved_p = p;
        
     if (*p == '@')
        {
         switch (*(++p))
            {
             case 'h' : 
                strcpy(temp_str, (dg.dg_heads > 1)? "ds":"ss");
                break;
             case 'H' : 
                sprintf(temp_str, "%d", dg.dg_heads);
                break;
             case 'd' :
                strcpy(temp_str, datarates_str[xdg.dg_odatarate]);
                break;
             case 'f' :
                strcpy(temp_str, dg.dg_fm? "fm":"mfm");
                break;
             case 'c' :
                sprintf(temp_str, "%d", dg.dg_cylinders);
                break;
             case 't' :
                sprintf(temp_str, "%d", dg.dg_sectors);
                break;
             case 's' :
                sprintf(temp_str, "%d", dg.dg_secsize);
                break;
             case '@' :
                strcpy(temp_str, "@");
                break;
             case 'u' :
                use_upper = 1;
                break;
             case 'l' :
                use_upper = 0;
                break;
             case 'm' :
                if (disk.mediadesc != -1)
                   sprintf(temp_str, "%02x", disk.mediadesc);
                break;
             case 'n' :
                strcpy(temp_str, xdg.dg_format_name);
                break;
             case '0' :
                if (disk.mediadesc != -1)
                   strcpy(job, "@h@d_@f_@cx@tx@s_@m@z");
                else
                   strcpy(job, "@h@d_@f_@cx@tx@s@z");                   
                p = job - 1;
                break;
             case '1' :
                if (disk.mediadesc != -1)             
                   strcpy(job, "@n_@h@d_@f_@cx@tx@s_@m@z");
                else
                   strcpy(job, "@n_@h@d_@f_@cx@tx@s@z");                   
                p = job - 1;
                break;
             case '2' :
                strcpy(job, "@n_@h@d_@f_@cx@tx@s@z");
                p = job - 1;
                break;
             case '3' :
                strcpy(job, "@n_@h@d_@cx@tx@s@z");
                p = job - 1;
                break;
             case '4' :
                strcpy(job, "@n_@cx@tx@s@z");
                p = job - 1;
                break;
             case '5' :
                strcpy(job, "@cx@tx@s@z");
                p = job - 1;
                break;
             case 'r' :
                strcpy(job, ".fdd-@c-@H-@t-@s@z");
                p = job - 1;
                break;
             case 'z' : // terminate a concatened sequence
                *job = 0;
                p = saved_p + 1;
                break;
             default :
                printf("output_filename_substitution(): "
                "unrecognised substitution character - %c\n", *p);
                return -1;
            }

         if (use_upper)
            toupper_string(temp_str, temp_str);

         strcat(subs_ofile_name,  temp_str);

         l = l + strlen(temp_str);
         p++;

         *temp_str = 0;
        }
     else
        // insert a normal character (no case conversion)
        {
         subs_ofile_name[l++] = *(p++);
         subs_ofile_name[l] = 0;
        }
    }

 strcpy(ofile_name, subs_ofile_name);

 if (disk.verbose > 1)
    printf("output_filename_substitution(): %s\n", subs_ofile_name);

 return 0;
}

//==============================================================================
// Open output drive.
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
static int open_output_drive (void)
{
 dsk_err_t dsk_err = DSK_ERR_OK;

 if (disk.verbose > 1)
    printf("open_output_drive()\n");

 // open the output file if one is specified
 if (*ofile_name)
    {
     if (output_filename_substitution() != 0)
        return -1;
    
     // for now just checks the output type
     if (check_output_drive() == -1)
        return -1;
        
     // check if destination file already exists and what to do
     disk.overwrite = overwrite_permission((disk.count < 0)? 1:2, ofile_name);
     if (disk.overwrite != 1)
        return -1;

     dsk_err = create_output_drive();
     if (dsk_err != DSK_ERR_OK)
        return -1;
    }
 return 0;
}

//==============================================================================
// Open input and output disk/image drives.
//
// Opens input drive to a default format or another if specified in
// disk.format.
//
// The order for searching formats is as follows:
// 1. ubeedisk built-in formats
// 2. libdsk built-in formats
// 3. libdskrc file formats
//
//   pass: void
// return: int                          0 if successful else -1
//==============================================================================
static int open_drives (void)
{
 dsk_err_t dsk_err = DSK_ERR_OK;

 dsk_format_t fmt; 
 dsk_cchar_t fdesc;
 dsk_cchar_t fname;
 
 // set default common geometry values
 format_set("default", 0);
 
 // open an input file if one is specified
 if (*disk.ifile)
    {
     // for now just checks the input type
     if (check_input_drive() == -1)
        return -1;
    
     dsk_err = open_input_drive();
     if (dsk_err != DSK_ERR_OK)
        return -1;
    }     

 // if scanning/speed/clean then no need for any special format
 if (disk.disk == UBEEDISK_SCAN ||
     disk.disk == UBEEDISK_SPEED ||
     disk.disk == UBEEDISK_CLEAN
    )
    return 0;   

 if (! disk.detect && ! *disk.format)
    {
     printf(APPNAME": no disk format! use --detect or --format options.\n");
     return -1;
    } 

 // try and get ubeedisk to detect the input disk format if detection enabled
 if (disk.detect)
    {
     // if a format detected it must set and report the type
     if (format_detect_disk() == 0)
        {
         if (open_output_drive() != 0)
            return -1;
         // now check the format's attributes
         return check_input_drive() | check_output_drive();
        }
     return -1;        
    }

 // check if the format is an internal type provided by ubeedisk
 if (*disk.format)
    {
     // if we find the format
     if (format_set(disk.format, 1) != -1)
        {
         if (open_output_drive() != 0)
            return -1;

         // now check the format's attributes
         return check_input_drive() | check_output_drive();
        }
    }
     
 // lastly check the formats built into LibDsk
 int found = 0;
 fmt = FMT_180K;
 while (dg_stdformat(NULL, fmt, &fname, NULL) == DSK_ERR_OK && ! found)
    {
     if (! strcmp(disk.format, fname))
        found = 1;
     else
        ++fmt;
    }

 if (! found)
    {   
     printf(APPNAME": format name '%s' not recognised.\n", disk.format);
     return -1;
    }
   
 // initialise disk geometry                                                       
 dsk_err = dg_stdformat(&dg, fmt, NULL, &fdesc);

 if (dsk_err == DSK_ERR_OK)
    {
     if (disk.verbose)
        printf("\nLibDsk/libdskrc format selected:\n%s: %s\n",
               fname, fdesc);
    } 
 else   
    {
     printf(APPNAME": format name '%s' not recognised.\n", fdesc);
     return -1;
    }

 if (open_output_drive() != 0)
    return -1;

 return check_input_drive() | check_output_drive();
}

//==============================================================================
// Reset input drive and settings for PC Floppy disks. (Work around #1)
//
// LibDsk (PC driver) seems to get confused and won't recover if there is no
// disk is in the drive when it tries to read it (Input/output error), this
// may also be caused by a cleaning disk as it won't have any track
// information, it may also be caused by no index hole being detected (just
// don't know).  Closing and reopening the input disk corrects for this on a
// PC Floppy.
//
// This function may also be called to overcome other problems when the
// drive is not ready.
//
// This process can be disabled by using --fdwa1=off
//
//   pass: void
// return: void                         may use exit(EXIT_FAILURE)
//==============================================================================
void close_reopen_pc_floppy_input_drive (void)
{
 dsk_err_t dsk_err = DSK_ERR_OK;

 // if not a host PC floppy then return
#ifdef WIN32
 if (strcmp("ntwdm", disk.itype) != 0)
#else
 if (strcmp("floppy", disk.itype) != 0)
#endif
    return;

 if (disk.verbose > 1)
    printf(APPNAME": close_reopen_pc_floppy_input_drive()\n");

 if (! disk.fd_workaround1)
    return;

 if (disk.verbose > 1)
    printf(APPNAME": close_reopen_pc_floppy_input_drive()\n");

 if (dsk_close(&idrive) != DSK_ERR_OK)
    {
     printf(APPNAME": close_reopen_pc_floppy_input_drive() - %s\n",
     dsk_strerror(dsk_err));
     // de-initialise the functions module
     functions_deinit();
     exit(EXIT_FAILURE);
    }
    
 idrive = NULL;

 if (open_input_drive() != 0)
    {
     // de-initialise the functions module
     functions_deinit();
     exit(EXIT_FAILURE);
    }

 override_values();
}

//==============================================================================
// Home and reset input drive and settings.  (Work around #2)
// See close_reopen_pc_floppy_input_drive() notes.
//
// This is a hack to handle problems with System and/or LibDsk.  The exact
// problem is not currently known nor what systems it occurs on (i.e
// win32/Linux).
//
// It appears that whenever a read function such as a dsk_psecid() returns a
// disk input/output error (i.e.  no disk in drive or error on disk) then no
// further disk access is possible without first closing the input drive
// then re-opening it.
//
// The problem with this hack is that it slows things down and means that
// the disk head has to be homed which is not ideal when trying to recover
// old floppy media.
//
// This process can be disabled by using --fdwa2=off
//
//   pass: DSK_FORMAT *sector_id
// return: dsk_err_t                    may use exit(EXIT_FAILURE)
//==============================================================================
static dsk_err_t home_and_reset_input_drive_and_settings (DSK_FORMAT *sector_id)
{
 dsk_err_t dsk_err;
 
 if (! disk.fd_workaround2)
    return DSK_ERR_OK;

 if (disk.verbose > 1)
    printf(APPNAME": home_and_reset_input_drive_and_settings()\n");

 // seek to track #0
 dsk_err = dsk_psecid(idrive, &dg, 0, 0, sector_id);

 // if not a host PC floppy then return
#ifdef WIN32
 if (strcmp("ntwdm", disk.itype) != 0)
#else
 if (strcmp("floppy", disk.itype) != 0)
#endif
    return DSK_ERR_OK;

 if (dsk_close(&idrive) != DSK_ERR_OK)
    {
     printf(APPNAME": home_and_reset_input_drive_and_settings() - %s\n",
     dsk_strerror(dsk_err));
     // de-initialise the functions module
     functions_deinit();
     exit(EXIT_FAILURE);
    }
    
 idrive = NULL;

 if (open_input_drive() != 0)
   {
    // de-initialise the functions module
    functions_deinit();
    exit(EXIT_FAILURE);
   }

 override_values();
 
 return dsk_err;
}

//==============================================================================
// Enter a disk description.
//
// Up to DESC_LINES lines may be entered.  Terminate by entering a '.' on a
// line by itself.  The last line always contains '.\n' by iteself.
//
//   pass: void
// return: int                          0 if normal exit, else -1 if quit
//==============================================================================
static int enter_disk_description (void)
{
 int i;

 // We will be prompted for a disk description if the following rule is true:
 //
 // If the 'info' file is enabled (default=enabled) AND
 // options --entdesc=on (default=on) AND
 // options --diskdesc OR --diskdescf have NOT been specified AND
 // The output type is NOT in the 'no_desc_otypes[]' list.

 if (! disk.info_file || ! disk.enter_desc ||
 string_search(no_desc_otypes, disk.otype) != -1)
    return 0;
 
 description[0][0] = 0; 
 
 printf("\n"
        "Enter a disk description.  A '.' on a line by itself ends input.\n"
        "A '*' by itself quits (no disk copy made). Maximum %d lines\n"
        "allowed. (Cut and paste from a text editor to make this easier)"
        "\n\n", DESC_LINES);

 for (i = 0; i < DESC_LINES; i++)
    {
     if (fgets(description[i], DESC_CHARS, stdin)) {} // ignore result 
     if (strcmp(".\n", description[i]) == 0)
        break;
     if (strcmp("*\n", description[i]) == 0)
        return -1;
    }
 description[i][0] = 0;
 return 0;
}

//==============================================================================
// Get a disk description from a text file.
//
// Prompting for a disk description will be disabled.
//
// This works the same way as for the enter_disk_description() function
// except takes the input from a file specified with an option.
//
//   pass: char *s                      disk description file name
// return: int                          -1 if error, else 0
//==============================================================================
int file_disk_description (char *s)
{
 FILE *fp;
 int i;

 disk.enter_desc = 0;  // disable prompting for a disk description.

 description[0][0] = 0; 

 fp = fopen(s, "r");
 if (! fp)
    return -1;
    
 for (i = 0; i < DESC_LINES; i++)
    {
     if (fgets(description[i], DESC_CHARS, fp)) {} // ignore result 
     if (strcmp(".\n", description[i]) == 0)
        break;
    }
 description[i][0] = 0;

 fclose(fp);
 return 0;
}

//==============================================================================
// Process a disk description line of text passed from an option.
//
// This function is called repeatedly for each line of text to be added to
// the disk description.  Prompting for a disk description will be disabled.
//
// Up to DESC_LINES lines may be entered and each line may contain up to
// DESC_CHARS characters.  Any excess characters will be lost so it will be
// up to the caller to make sure lines are passed one at a time and not as
// one long string.
//
// The last line must contain '.\n' by iteself and is inserted after each
// call to this function and overwritten on the next call.
//
//   pass: char *s                      disk description string
// return: void
//==============================================================================
void process_disk_description (char *s)
{
 disk.enter_desc = 0;  // disable prompting for a disk description.

 if (diskdesc_line == DESC_LINES)
    return;
    
 strncpy(&description[diskdesc_line][0], s, DESC_CHARS);
 strcat(&description[diskdesc_line++][0], "\n");

 // terminate the lines of input with a '.\n' on a line by itself
 strcpy(&description[diskdesc_line][0], ".\n");
}

//==============================================================================
// Set start and finish cylinder/track values.
//
// Values are only set if NULL is not passed as pointer.
//
// The values are calculated based on the --sfmode which may be cylinder (c=0)
// or track mode (t=1)
//
// Converting cylinders to tracks and tracks to cylinders:
//
// sfmode='c': Input START values as cylinders
// track = start * heads
//
// sfmode='t': Input START values as tracks
// cylinder = start / heads
//
// sfmode='c': Input FINISH values as cylinders
// track = (finish * heads) + heads - 1
//
// sfmode='t': Input FINISH values as tracks
// cylinder = finish / heads
// 
// Note: When the input type is cylinders the finish track is the logical
// track coresponding to the cylinder and head 1 (if DS).  So if cylinder=0
// the corresponding logical track on a double side disk finishes at 1.
//
//   pass: int *scyl                    start cylinder ptr
//         int *fcyl                    finish cylinder ptr
//         int *strk                    start track ptr
//         int *ftrk                    finish track ptr
// return: void
//==============================================================================
static void set_start_finish (int *scyl, int *fcyl, int *strk, int *ftrk)
{
 // starting cylinder/track
 if (disk.start < 0)
    {
     if (scyl)
        *scyl = 0;
      if (strk)
        *strk = 0;
    }
 else
    {
     if (scyl)
        *scyl = (disk.sfmode == 0)? disk.start : (disk.start / dg.dg_heads);
     if (strk)
        *strk = (disk.sfmode == 0)? (disk.start * dg.dg_heads) : disk.start;
    }

 // set finish to last track/cylinder if -1
 if (disk.finish < 0)
    {
     if (fcyl)
        *fcyl = dg.dg_cylinders - 1;
     if (ftrk)        
        *ftrk = dg.dg_cylinders * dg.dg_heads - 1;
    }
 else
    {
     if (fcyl)
        *fcyl = (disk.sfmode == 0)? disk.finish : (disk.finish / dg.dg_heads);
     if (ftrk)
        *ftrk = (disk.sfmode == 0)?
        (disk.finish * dg.dg_heads + (dg.dg_heads - 1)) : disk.finish;
    }
}

//==============================================================================
// Set special disk values.
//
//   pass: dsk_pcyl_t cyl               cylinder number
//         dsk_phead_t head             physical side of disk
//         int secsize                  sector size
// return: void
//==============================================================================
static void set_special_disk (dsk_pcyl_t cyl, dsk_phead_t head, int secsize)
{
 // set special disk values using a call-back function if defined
 if (xdg.ssd_cb)
    (*xdg.ssd_cb)(cyl, head, secsize);
}

//==============================================================================
// Disk clean.
//
// This may be used to clean the drive heads using a special cleaning disk.
//
// As there is no low level control disk cleaning will simply try to read a
// sector ID at given a given cylinder and side.  The retry value will be
// set as low as possible.  After the call times-out the process will be
// repeated for other tracks.
//
// This function may be called as a command line process or while a disk is
// being copied.
//
// If the command method is not 'c' then the disk head will be moved back to
// the last cylinder value before asking for the media disk to be put back. 
// This prevents moving the head across the disk causing uneccessary wear.
//
//   pass: int method                   pass 'c' if command line
//         int cyl_last                 last cylinder position
// return: int                          0 if no error, else -1
//==============================================================================
static int disk_clean (int method, int cyl_last)
{
 char user[100];

 DSK_FORMAT result; 
 dsk_pcyl_t cyl;
 int iterations = 0;
 int done = 0;
 
 // check that the input drive is a floppy or remote type
 if (string_search(no_info_file, disk.itype) == -1)
    {
     printf("\n"APPNAME": input device must be a floppy drive for cleaning!\n");
     return -1;
    } 

 if (method == 'c')
    {
     // set input type based on input name
     set_xtype_xfile(disk.ifile, disk.itype);

     if (open_drives() == -1)
        return -1;

     if (override_values() != DSK_ERR_OK)
        return -1;

     if (disk.verbose)
        printf("\n");
    }
     
 // if the input driver does not support the dsk_psecid() function then
 // exit immediately as there is no point in continuing.
 if (! input_sup.psecid)
    {
     printf(APPNAME": input device does not support the"
            " dsk_psecid() function.\n");
     return -1;
    } 

 // set 0 retries for the input drive (we want it to give up ASAP)
 dsk_set_retry(idrive, 0);

 // we must set some valid values if this is the command line method
 if (method == 'c')
    {
     dg.dg_fm = 0;
     dg.dg_heads = 255;
     dg.dg_noskip = 1;
     dg.dg_datarate = RATE_SD;
    }
 else
    {
     printf("\nInsert the cleaning disk then press ENTER to start the"
            " cleaning process.\n");
     if (fgets(user, sizeof(user)-2, stdin))
        {} // ignore result
    }   

 printf("Press any key to exit (exits after 5 passes)...\n\n");

 while (iterations++ != 5)
    {
     printf("Cleaning pass: %d/5\n", iterations); 

     // step in 5 cylinders at a time
     for (cyl = 0; cyl < 40;)
        {
         if (disk.verbose > 1)
            printf("Moving head to cylinder %2d.\n", cyl); 
         done = get_key() != -1;
         if (done)
            break;
         dsk_psecid(idrive, &dg,  cyl, 0, &result);
         sleep_ms(100);
         cyl += 5;
        }

     if (done)
        break;

     // step back 5 cylinders at a time
     for (cyl = 30; cyl > 0;)
        {
         if (disk.verbose > 1)
            printf("Moving head to cylinder %2d.\n", cyl); 
         done = get_key() != -1;
         if (done)
            break;
         dsk_psecid(idrive, &dg,  cyl, 0, &result);
         sleep_ms(100);
         cyl -= 5;
        }

     if (done)
        break;
    }

 if (method == 'c')
    {
     printf("\nRemove the cleaning disk from the drive.\n"); 
     close_files();
     return 0;
    }

 // place head back on the same cylinder before finishing the clean
 if (disk.verbose > 1)
    printf("Moving head back to cylinder %2d.\n", cyl_last);
 dsk_psecid(idrive, &dg,  cyl_last, 0, &result);
 sleep_ms(100);

 printf("\nRemove the cleaning disk and REPLACE disk then press ENTER"
 " to return to the MENU.\n");

 // flush any pending keys;
 while (get_key() != -1)
    ;

 if (fgets(user, sizeof(user)-2, stdin))
    {} // ignore result

 // must close/reopen the input drive and restore settings if PC floppy
 close_reopen_pc_floppy_input_drive();

 // need to put this value back  
 dsk_set_retry(idrive, disk.retries_l1);
 
 return 0;
}

//==============================================================================
// Disk scan head options.
//
// --scanhead=s[,f]
//
// Set the head range to be scanned when using the --disk=scan option, 's'
// is the head to start with, 'f' is an optional finish head otherwise
// finish will be the same as 's'.  Default values are are 0,1.
//
//   pass: char *parms                  command parameters
// return: int                          0 if no error, else -1
//==============================================================================
int scan_option (char *parms)
{
 char s[100];
 char *c;
 int start;
 int finish;
 
 // get the head start number 's'
 c = get_next_parameter(parms, ',', s, &start, sizeof(s)-1);
 if (start < 0 || start > 255)
    return -1;
 
 disk.head_s = start;
 disk.head_f = start;

 // if no parameter given for the optional 'f' head then we are done
 if (! c)
    return 0;

 // get the head finish number 'f'
 c = get_next_parameter(c, ',', s, &finish, sizeof(s)-1);
 if (finish < 0 || finish > 255)
    return -1;

 // check that the finish head is higher or same as the start head
 if (finish < start)
    return -1;
    
 disk.head_f = finish;    

 // can't be any more sub-parameters
 if (c)
    return -1;

 return 0;
}

//==============================================================================
// Disk scan.
//
// Scans a disk with auto detection for data rate and encoding method.
//
// The output reported is from the sector ID on the disk and these values
// may differ to the physical Cylinder and Head values.
//
//   pass: void
// return: int                          0 if no error, else -1
//==============================================================================
static int disk_scan (void)
{
 // data rates arranged in order that works when a 360k in 1.2Mb drive it
 // should match up as RATE_DD when tested (don't have RATE_SD first!)
 dsk_rate_t datarates[4] = {RATE_DD, RATE_HD, RATE_SD, RATE_ED};

 int data_rates_khz[] =
 { 
  500, // RATE_HD (500 kHz)
  300, // RATE_DD (300 kHz)
  250, // RATE_SD (250 kHz)
  0    // NO  kHz
 };

 int rate_khz = 250;

 dsk_err_t dsk_err = DSK_ERR_OK;

 dsk_psect_t count;

 // LibDsk will allocate memory and set this pointer (dsk_ptrackids)
 DSK_FORMAT *result;
 
 DSK_FORMAT sector_id;

 dsk_pcyl_t cyl;
 dsk_phead_t head;
 int i;
 int dr;

 // set input type based on input name
 set_xtype_xfile(disk.ifile, disk.itype); 

 // check we have an input file name
 if (! *disk.ifile)
    {
     printf(APPNAME": use '--if' option to specify an input file.\n");
     return -1;
    }

 if (open_drives() == -1)
    return -1;

 // if the input driver does not support the dsk_ptrackids() function then
 // exit as there is no point in continuing.
 if (! input_sup.ptrackids)
    {
     printf(APPNAME": input device does not support the"
            " dsk_ptrackids() function.\n");
     return -1;
    } 

 if (override_values() != DSK_ERR_OK)
    return -1;

 if (disk.verbose)
    printf("\n");

 // set starting and finishing cylinder values
 set_start_finish(&cyl_start, &cyl_finish, NULL, NULL);

 // set retries for the input drive
 dsk_set_retry(idrive, disk.retries_l1);
 
 // The first time the head may need to be homed and this can cause a non
 // recoverable error so do a dummy read of T0, H0, if error close, delay and
 // open again.
 dg.dg_fm = 0;
 dg.dg_heads = 255;
 dg.dg_noskip = 1;
 dg.dg_datarate = datarates[0];
 
 // some drives need this or the disk head won't get positioned and we end
 // up with no data error! (must be done in order definded in function)
 dsk_err = home_and_reset_input_drive_and_settings(&sector_id);

 // must close/reopen the input drive and restore settings if PC floppy
 close_reopen_pc_floppy_input_drive();

 // must close and reopen the input drive and restore settings if error.
 if (dsk_err != DSK_ERR_OK)
    {
     if (disk.verbose > 1)
        printf("disk_scan(): sleep_ms(2000)!\n");
     sleep_ms(2000);
     // must close/reopen the input drive and restore settings if PC floppy
     close_reopen_pc_floppy_input_drive();
    }

 // force the first read
 dsk_err = 1;

 for (cyl = cyl_start; cyl <= cyl_finish; cyl++)
    {
     for (head = disk.head_s; head <= disk.head_f; head++)
        {
         // if the last read was not an error
         if (dsk_err == DSK_ERR_OK)
            dsk_err = dsk_psecid(idrive, &dg, cyl, head, &sector_id);
         else
            // check all data rates with MFM (0) mode first then FM (1)
            for (dg.dg_fm = 0; dg.dg_fm < 2; dg.dg_fm++)
               {
                // find the data rate starting with the most likely first
                for (dr = 0; dr < 4; dr++)
                   {
#if 0                   
                    printf("dr=%d drive type=%c\n", dr, disk.idrive_type);
#endif
                    dg.dg_datarate = datarates[dr];
                    dsk_err = dsk_psecid(idrive, &dg, cyl, head, &sector_id);
                    if (! dsk_err)
                       break;
                   }    
                if (! dsk_err)
                   break;
               }

         if (dsk_err)
            printf("Nothing detected: Cylinder: %3d  Head: %2d"
                   "  Result: %s\n\n", cyl, head, dsk_strerror(dsk_err));
         else
            {
             dsk_err = dsk_ptrackids(idrive, &dg, cyl, head, &count, &result);

             if (dsk_err == DSK_ERR_OK)
                {
                 // data rate will be one of 125, 150, 250, 300 or 500 kHz
                 // if 1.2Mb drive and 250kHz detected then set rate to 300 kHz
                 if (disk.idrive_type == 'h' && dg.dg_datarate == RATE_SD)
                    rate_khz = 300;
                 else
                    // else rate is 250 or 500 kHz
                    rate_khz = data_rates_khz[dg.dg_datarate];
                    
                 // if FM mode then halve the data rate value
                 if (dg.dg_fm)
                    rate_khz /= 2;

                 printf("Cylinder:%4d  Head:%2d  Sectors:%4d"
                        "  Data rate kHz: %03d  Encoding: %s\n",
                        cyl, head, count, rate_khz,
                        encmode_str[dg.dg_fm]);
                 printf("------------------------------------"
                        "-----------------------------------\n");
         
                 for (i = 0; i < count; i++)
                    {
                     sector_id = result[i];
                     printf("Track: %3d  Side: %2d  Sector: %3d"
                            "  Bytes/sector: %4d\n",
                            sector_id.fmt_cylinder,
                            sector_id.fmt_head,
                            sector_id.fmt_sector,
                            sector_id.fmt_secsize);
                    }
                 // free the memory that LibDsk created when dsk_ptrackids()
                 dsk_free(result);
                 printf("\n");
                }
             else
                {
                 // depending on the error terminate the scan process
                 // printf(APPNAME": error reading track IDs: %s\n",
                 // dsk_strerror(dsk_err));
                 // return -1;
                }
            }

         // allow the operator to pause, resume, abort, and clean
         if (pause_menu(1, cyl) == DSK_ERR_ABORT)
            {
             close_files();
             return 0;
            }
        }
    }
 close_files();
 return 0;
}

//==============================================================================
// Format a disk/image(s).
//
// A range of logical tracks can be formatted.
//
//   pass: void
// return: int                          0 if no error, else -1
//==============================================================================
static int disk_format (void)
{
 dsk_err_t dsk_err = DSK_ERR_OK;
 dsk_pcyl_t cyl;
 dsk_ltrack_t trk; 
 dsk_phead_t head;

 char user[10];

 // must make sure there is no input file or strange things happen!
 *disk.ifile = 0;

 // set output type based on output name
 set_xtype_xfile(disk.ofile, disk.otype); 

 // check we have an output file name.
 if (! *disk.ofile)
    {
     printf(APPNAME": use --of option to specify an output file.\n");
     return -1;
    }
 
 if (disk.noformat)
    {
     printf("\n"APPNAME": '--noformat' option is enabled, no format!\n");
     return -1;
    } 

 strcpy(ofile_name, disk.ofile);

 if (open_drives() == -1)
    return -1;

 if (disk.verbose)
    printf("\nFormatting disk/image '%s'\n", ofile_name);

 // prompt the user to confirm format if no --force option specified
 if (! disk.force)
    {
     user[0] = 0;
     printf("\nWARNING: To format type 'format', anything else aborts: ");
     if (fgets(user, sizeof(user), stdin)) {} // ignore result 
     if (strcmp("format\n", user) != 0)
        {
         printf("Format aborted.\n");
         return 0;
        }
    }

 // set data rate and MFM/FM mode for output
 dg.dg_datarate = xdg.dg_odatarate;
 dg.dg_fm = xdg.dg_ofm;

 // if the output driver does not support the dsk_pformat() function then
 // exit as there is no point in continuing.
 if (! output_sup.pformat)
    {
     printf(APPNAME": output device does not support the"
            " dsk_pformat() function.\n");
     return -1;
    } 

 if (override_values() != DSK_ERR_OK)
    return -1;

 if (disk.verbose)
    printf("\n");

 // set starting and finishing cylinder values
 set_start_finish(&cyl_start, &cyl_finish, &trk_start, &trk_finish);

 dsk_set_retry(odrive, 10);

 // format the requested logical tracks
 for (trk = trk_start; trk <= trk_finish; trk++)
    {
     cyl = trk / dg.dg_heads;
     head = trk % dg.dg_heads;

     if (xdg.dg_secbase2c != -1 && cyl >= xdg.dg_secbase2c)
        dg.dg_secbase = xdg.dg_secbase2s;
     else   
        dg.dg_secbase = xdg.dg_secbase1s;
        
     // handle special disk formats
     set_special_disk(cyl, head, -1);

     if (disk.verbose)         
        {
         printf("\rCylinder: %02d/%02d Head: %d/%d",
                cyl, dg.dg_cylinders-1, head, dg.dg_heads-1);
         fflush(stdout);
        } 

     switch (disk.forceside)
        {
         case 0 : // off
            dsk_err = format_track(cyl, head,
            (head & (1 ^ xdg.dg_side1as0)) + xdg.dg_sideoffs);  
            break;
         case 1 : // on
            // force side ID to be equal to the physical side
            dsk_err = format_track(cyl, head, head);
            break;
         case 2 : // 00
            dsk_err = format_track(cyl, head, 0);
            break;
         case 3 : // 01
            if (head == 0)
               dsk_err = format_track(cyl, head, 0);
            else
               dsk_err = format_track(cyl, head, 1);
            break;
         case 4 : // 10
            if (head == 0)
               dsk_err = format_track(cyl, head, 1);
            else
               dsk_err = format_track(cyl, head, 0);
            break;
         case 5 : // 11
            dsk_err = format_track(cyl, head, 1);
            break;
        }

     if (dsk_err != DSK_ERR_OK)
        return -1;
     
     // allow the operator to pause, resume and abort
     if (pause_menu(2, cyl) == DSK_ERR_ABORT)
        {
         printf("\n");
         return 0;
        }
    }

 if (disk.verbose)
    printf("\n\nFormat completed.\n");
 return 0;
}

//==============================================================================
// Disk speed.
//
// Measure rotational speed of disk.  This is done by repeatedly reading the
// same sector and measuring the time between the first and last read.  A
// standard disk must be used that does not have the same sector on the
// track more than once.
//
//   pass: void
// return: int                          0 if no error, else -1
//==============================================================================
static int disk_speed (void)
{
 // data rates arranged in order that works when a 360k in 1.2Mb drive it
 // should match up as RATE_DD when tested (don't have RATE_SD first!)
 dsk_rate_t datarates[4] = {RATE_DD, RATE_HD, RATE_SD, RATE_ED};

 // these must be kept in this order
 char datarates_str[4][5] = {" 500", " 300", " 250", "1000"};

 dsk_err_t dsk_err = DSK_ERR_OK;

 dsk_psect_t count;
 DSK_FORMAT *result;
 DSK_FORMAT sector_id; 

 dsk_pcyl_t cyl;
 dsk_phead_t head;
 dsk_psect_t psect;
 int dr;

 int i;
 int iterations = 0;
 int rpm;
 int64_t ms_start;
 int64_t ms_finish;
 int ms_total;

 // set input type based on input name
 set_xtype_xfile(disk.ifile, disk.itype); 

 // check we have an input file name
 if (! *disk.ifile)
    {
     printf(APPNAME": use '--if' option to specify an input file.\n");
     return -1;
    }

 if (open_drives() == -1)
    return -1;

 // if the input driver does not support the dsk_psecid() function then
 // exit as there is no point in continuing.
 if (! input_sup.psecid)
    {
     printf(APPNAME": input device does not support the"
            " dsk_psecid() function.\n");
     return -1;
    } 

 if (override_values() != DSK_ERR_OK)
    return -1;

 if (disk.verbose)
    printf("\n");

 // set starting and finishing cylinder values. Only the start value is used.
 set_start_finish(&cyl_start, NULL, NULL, NULL);
 cyl = cyl_start;

 // speed testing can be instructed what side of the disk to read with the
 // --iside option.  If set to default of -1 then uses side 0.
 if (disk.iside == -1)
    head = 0;
 else
    head = disk.iside;

 // set retries for the input drive
 dsk_set_retry(idrive, disk.retries_l1);
 
 // The first time the head may need to be homed and this can cause a non
 // reverable error so do a dummy read of T0, H0, if error close, delay and
 // open again.
 dg.dg_fm = 0;
 dg.dg_heads = 255;
 dg.dg_noskip = 1;
 dg.dg_datarate = datarates[0];
 dsk_err = dsk_psecid(idrive, &dg, 0, 0, &sector_id);

 // must close and reopen the input drive and restore settings if error.
 if (dsk_err != DSK_ERR_OK)
    {
     sleep_ms(2000);
     // must close/reopen the input drive and restore settings if PC floppy
     close_reopen_pc_floppy_input_drive();
    }
 
 // check all data rates with MFM (0) mode first then FM (1)
 for (dg.dg_fm = 0; dg.dg_fm < 2; ++dg.dg_fm)
    {
     // find the data rate starting with the most likely first
     for (dr = 0; dr < 4; ++dr)
        {
         dg.dg_datarate = datarates[dr];
         dsk_err = dsk_psecid(idrive, &dg, cyl, head, &sector_id);
         if (! dsk_err)
            break;
        }       
     if (! dsk_err)
        break;
    }

 if (dsk_err)
    {
     printf("Nothing detected: Cylinder: %3d  Head: %2d  Result: %s\n",
     cyl, head, dsk_strerror(dsk_err));
     printf("Note: A disk that has at minimum track 0 and side 0 formatted"
            " must be in the drive.\n");
     close_files();         
     return -1;
    }

 dsk_err = dsk_ptrackids(idrive, &dg, cyl, head, &count, &result);

 if (dsk_err)
    {
     close_files();         
     return -1;
    }

 printf("Cylinder: %3d  Head: %2d  Sectors: %2d"
        "  Data rate kHz: %s  Encoding: %s\n",
        cyl, head, count, datarates_str[dg.dg_datarate],
        encmode_str[dg.dg_fm]);
 
 psect = sector_id.fmt_sector;
 
 printf("Disk rotations for results: 5  Sector number sampled: %d\n\n", psect);

 // now that we have all the values from the unkown disk format we can read
 // the same sector over and over to find the rotational speed of the drive.
 printf("Press any key to pause (exits after 60 iterations)...\n\n");

 while (iterations++ != 60)
    {
     // allow the operator to pause, resume, abort, and clean
     if (pause_menu(1, cyl) == DSK_ERR_ABORT)
        break;

     // set the data rate back to what it was (in case of clean)        
     dg.dg_datarate = datarates[dr];

     // get the first occurrence of the sector
     for (;;)
        {
         dsk_err = dsk_psecid(idrive, &dg, cyl, head, &sector_id);
         if (psect == sector_id.fmt_sector || dsk_err != DSK_ERR_OK)
            break;
        }    
     ms_start = time_get_ms();

     // read in the same sector 5 ot 6 times depending on the drive type
     for (i = 0; i < 5; i++)
        {
         for (;;)
            {
             dsk_err = dsk_psecid(idrive, &dg, cyl, head, &sector_id);
             if (psect == sector_id.fmt_sector || dsk_err != DSK_ERR_OK)
                break;
            }
         }   

     ms_finish = time_get_ms();
     ms_total = ms_finish - ms_start;

     // add 0.5 to round the result up
     rpm = (int)(300.0 * (1000.0 / ms_total) + 0.5);

     printf("Iterations: %2d  Time (ms): %4d  RPM: %3d\n",
     iterations, ms_total, rpm);
    }

 close_files();
 return 0;
}

//==============================================================================
// Copy one disk/image(s).
//
// Retries (3 levels) - An explanation of how the 3 levels of retries works
// is provided here:
//
// LibDsk (level 1)
// ----------------
// A LibDsk retry level is set here to help make sure there is plenty of
// time for a sector to be read successufully at the LibDsk level.  This
// should not be set to a too low value as it causes many soft errors and
// possibly causes read problems on some disks (especially Microbee disks). 
// The value can be changed using the --retry-l1 option if required.
//
// read_sector() (level 2)
// -----------------------
// The 2nd level of retries are performed in read_sector() which can be
// changed using the --level-l2 option if required.  If the LibDsk retries
// fail then this retry level comes into play.
//
// read_sector_retry() (level 3)
// -----------------------------
// If level 2 fails to read a sector then this last level comes into play.
//
// If the Unattended mode is in use (default) then some more retries (with
// and without T0 seeks), ignore or abort operations will come into play. 
// The exact way this works depends on the count of previous errors and seek
// counts that have already occured.
//
// If interactive mode is in use then the operator has complete control and
// makes a selection when prompted for the next course of action.
//
//   pass: void
// return: int                          0 if no error, else -1
//==============================================================================
static int copy_one_disk (void)
{
 dsk_err_t dsk_err = DSK_ERR_OK;
 DSK_FORMAT sector_id; 

 uint8_t *p;
 dsk_pcyl_t cyl;
 dsk_ltrack_t trk; 
 dsk_phead_t head; 
 int xhead;

 char trk_str[5001]; 
 int trk_str_len;

 int xsecsize;
 int lsect;
 int psect;
 int aborted = 0;

 disk.write_error_count = 0;
 
 if (open_drives() == -1)
    return -1;

 if (override_values() != DSK_ERR_OK)
    return -1;

 // initialise the info file
 init_info_file();
 
 // enter a disk description
 if (enter_disk_description() == -1)
    return -1;

 if (disk.verbose) 
    printf("\nCopying disk/image '%s' to '%s'\n", disk.ifile, ofile_name);

 sect_errors_tot = 0;
 sect_retries_tot = 0;

 auto_retry_abort = 0;
 auto_cyl_last = -1;
 auto_head_last = -1;
 auto_seeked_count = 0;

 // set starting and finishing cylinder values
 set_start_finish(&cyl_start, &cyl_finish, &trk_start, &trk_finish);

#if 0
 printf("\ncyl_start=%d cyl_finish=%d trk_start=%d trk_finish=%d\n",
 cyl_start, cyl_finish, trk_start, trk_finish);
 return 0;
#endif

 // format starting tracks that are being skipped (LibDsk insists)
 if (trk_start > 0)
    {
     if (disk.verbose)
        printf("\n");
     format_track_range(0, trk_start-1);
    }

 if (disk.verbose)
    {
     if (disk.noformat || ! output_sup.pformat)
        printf("\nCopying data:\n");
     else
        printf("\nCopying data (with format):\n");
    }

 // some drives need this or the disk head won't get positioned and we end
 // up with no data error! (must be done in order definded in function)
 home_and_reset_input_drive_and_settings(&sector_id);
 
 if (disk.verbose > 1)
    report_dg(&dg);
 
 for (trk = trk_start; trk <= trk_finish; trk++)
    {
     cyl = trk / dg.dg_heads;
     head = trk % dg.dg_heads;

     if (xdg.dg_secbase2c != -1 && cyl >= xdg.dg_secbase2c)
        dg.dg_secbase = xdg.dg_secbase2s;
     else   
        dg.dg_secbase = xdg.dg_secbase1s;

     // read first available sector ID to get the side ID
     if (! aborted)
        dsk_err = read_sector_id(cyl, head, &xhead, &xsecsize);
     else
        {
         xhead = head + xdg.dg_sideoffs;
         xsecsize = dg.dg_secsize;
         dsk_err = DSK_ERR_OK;
        } 

     // handle special disk formats
     set_special_disk(cyl, head, xsecsize);
     
     // create the skew table everytime as the format may change
     create_skew_table(xdg.dg_skew_val, xdg.dg_skew_ofs, dg.dg_sectors);
     
     // format one track
     if (! disk.noformat && aborted != 2)
        {
         switch (disk.forceside)
            {
             case 0 : // off
                // follow the input sector header ID value (xhead) if a
                // true psecid supported or xdg.dg_sideoffs is set
                if (input_sup.psecid || xdg.dg_sideoffs)
                   dsk_err = format_track(cyl, head, xhead);
                else
                   // else the format's side1as0 value will determine
                   dsk_err = format_track(cyl, head, xhead &
                   (1 ^ xdg.dg_side1as0));
                break;
             case 1 : // on
                // force side ID to be equal to the physical side
                dsk_err = format_track(cyl, head, head);
                break;
             case 2 : // 00
                dsk_err = format_track(cyl, head, 0);
                break;
             case 3 : // 01
                if (head == 0)
                   dsk_err = format_track(cyl, head, 0);
                else
                   dsk_err = format_track(cyl, head, 1);
                break;
             case 4 : // 10
                if (head == 0)
                   dsk_err = format_track(cyl, head, 1);
                else
                   dsk_err = format_track(cyl, head, 0);
                break;
             case 5 : // 11
                dsk_err = format_track(cyl, head, 1);
                break;
            }
        }

     // read and write one complete track
     buffered_cylinder = -1;
     buffered_head = -1;
     
     trk_str_len = 0;

     if (dsk_err == DSK_ERR_OK)
        {
         // read the track
         for (lsect = 0; lsect < dg.dg_sectors; lsect++)
            {
             // get the skewed physical sector number
             psect = skew_table[lsect] + dg.dg_secbase;
         
             // read sectors and write to info file
             if (! aborted)
                {
                 dsk_err = read_sector_retry(cyl, head, xhead, lsect);
                 if (disk.verbose > 1)
                    trk_str_len += sprintf(trk_str + trk_str_len,
                    " %2d", psect);
                }
             else
                {
                 // calculate the buffer pointer location
                 p = buf + dg.dg_secsize * (psect - dg.dg_secbase);

                 // fill the sector buffer location with 0xff
                 memset(p, 0xff, dg.dg_secsize);
                 
                 // unread sector status
                 sect_errors_tot++;
                 sect_retry_count = -2;
                }

             // log the status of each sector read
             info_file_entry(cyl, head, lsect);

             // if read sector retry returned an abort repsonse
             if (dsk_err == DSK_ERR_ABORT)
                {
                 // set the abort method depending on the output type
                 aborted = (string_search(no_info_file, disk.otype) == -1)?
                 1 : 2;
                 dsk_err = DSK_ERR_OK;
                } 
            }

         // output the physcial sector string                
         if (disk.verbose > 1 && ! aborted)
            printf("  %s\n", trk_str);

         // write the buffered track
         if (aborted != 2)
            write_buffered_track(cyl, cyl, head, xhead);
        }
    }

 if (disk.verbose)
    printf("\n");

 if (! disk.nofill)
    {
     // format remaining tracks that are being skipped
     if (trk_finish < (dg.dg_cylinders * dg.dg_heads - 1))
        {
         if (disk.verbose)
            printf("\n");
         format_track_range(trk_finish+1, (dg.dg_cylinders * dg.dg_heads - 1));
        }
    }

 // output abort abort message
 if (aborted)
    {
     if (disk.unattended)
        printf("\nUnattended mode aborted this disk copy as the"
               "maximum number of sector\n"
               "retries were exceeded.  You may need to use the"
               " --unattended-rab option\n"
               "to increase the maximum limit.\n");
     else
        printf("\nUser aborted this disk copy.\n");
    }

 return create_info_file();    
}

//==============================================================================
// Copy disk/image(s).
//
//   pass: void
// return: int                          0 if no error, else -1
//==============================================================================
static int copy_disks (void)
{
 char user[100];
 char b_str[1000];
 char e_str[1000];
 char *p;
 int i;
 int l;
 int count;
 int res;

 overwrite_flag = -1;
 
 // set input and output types based on input and output names
 set_xtype_xfile(disk.ifile, disk.itype);
 set_xtype_xfile(disk.ofile, disk.otype); 

 // check we have an input file name
 if (! *disk.ifile)
    {
     printf(APPNAME": use '--if' option to specify an input file.\n");
     return -1;
    }

 // check we have an output file name.
 if (! *disk.ofile)
    {
     printf(APPNAME": use '--of' option to specify an output file.\n");
     return -1;
    }

 // copy one disk then exit
 if (disk.count < 0)
    {
     strcpy(ofile_name, disk.ofile);
     if (strcmp(disk.ifile, ofile_name) == 0)
        {
         printf(APPNAME": input and output file names must be different!\n");
         return -1;
        }
     
     res = copy_one_disk();
     
     // if 'no' was selected (not an error but we are done)
     if (disk.overwrite != 1)
        return 0;

     if (res == 0 && disk.verbose)
        {
         printf("\n");
         printf("Sector read errors: %d\n", sect_errors_tot);
         printf("       Retry total: %d\n", sect_retries_tot);
        } 
     close_files();
     return res;
    }

 // prompt user for each disk to be copied (verbose has no affect here)
 count = disk.count;
 
 p = strstr(disk.ofile, ".");
 if (p)
    {
     strncpy(b_str, disk.ofile, (p - disk.ofile));
     b_str[(p - disk.ofile)] = 0;
     strcpy(e_str, p);
    }
 else
    {
     strcpy(b_str, disk.ofile);
     e_str[0] = 0;
    } 

 for (;;)
    {
     printf("\n");

     sprintf(ofile_name, "%s%03d%s", b_str, count, e_str);

     if (strcmp(disk.ifile, ofile_name) == 0)
        {
         printf(APPNAME": input and output file names must be different!\n");
         return -1;
        }

     l = (80 - (strlen(ofile_name) + 2)) / 2;
     for (i = 0; i < l; i++)
        putchar('=');
     printf(" %s ", ofile_name);
     for (i = 0; i < l; i++)
        putchar('=');
     printf("\n\n");

     // prompt the user to insert disk or exit
     user[0] = 0;
     while ((strcmp("\n", user) != 0) && (strcmp("e\n", user) != 0) &&
        (strcmp("E\n", user) != 0))
        {
         printf("Place next disk into drive '%s' and press ENTER or 'e'"
         " to exit ?: ", disk.ifile);
         if (fgets(user, sizeof(user)-2, stdin))
            {} // ignore result 
        }
     if (strcmp("\n", user) == 0)
        {
         res = copy_one_disk();
         
         // if 'skip all' was selected (not an error but we are done)
         if (disk.overwrite == 2)
            return 0;
             
         if (res == 0 && disk.verbose)
            {
             printf("\n");
             printf("Sector read errors: %d\n", sect_errors_tot);
             printf("       Retry total: %d\n", sect_retries_tot);
            }
         
         close_files();
         count++;
        }
     else
        return 0;
    } 
}

//==============================================================================
// Disk info.
//
// Dump geometry of the disk.  Does not show all values for formats that are
// dynamic types.
//
//   pass: void
// return: int                          0 if no error, else -1
//==============================================================================
static int dump_geometry (void)
{
 // set input type based on input name
 set_xtype_xfile(disk.ifile, disk.itype); 

 if (open_drives() == -1)
    return -1;

 printf(
 "\n"
 "       Sidedness: %d (%s)\n"
 "       Cylinders: %d\n"
 "           Heads: %d\n"
 "   Sectors/track: %d\n"
 "     Sector base: %d\n"
 "     Sector size: %d\n"
 " Input data rate: %d (%s)\n"
 "Output data rate: %d (%s)\n"
 "         R/W gap: %d\n"
 "      Format gap: %d\n"
 "         FM mode: %d (%s)\n"
 "   No multi mode: %d (%s)\n"
 "     No skip del: %d (%s)\n"
 "       iside1as0: %d\n"
 "       secbase1s: %d\n"
 "       secbase2c: %d\n"
 "       secbase2s: %d\n"
 "       Read skew: %d\n"
 "Read skew offset: %d\n"
 "   Special flags: %d\n\n",
 dg.dg_sidedness, sidedness[dg.dg_sidedness],
 dg.dg_cylinders,
 dg.dg_heads,
 dg.dg_sectors,
 dg.dg_secbase,
 dg.dg_secsize,
 xdg.dg_idatarate, datarates_str[xdg.dg_idatarate],
 xdg.dg_odatarate, datarates_str[xdg.dg_odatarate],
 dg.dg_rwgap,
 dg.dg_fmtgap,
 dg.dg_fm, noyes[dg.dg_fm],
 dg.dg_nomulti, noyes[dg.dg_nomulti],
 dg.dg_noskip, noyes[dg.dg_noskip],
 xdg.dg_side1as0,
 xdg.dg_secbase1s,
 xdg.dg_secbase2c,
 xdg.dg_secbase2s,
 xdg.dg_skew_val,
 xdg.dg_skew_ofs,
 xdg.dg_special
 );

 close_files();
 return 0;
}

//==============================================================================
// Main
//
//   pass: int argc                     number of arguments
//         char *argv[]                 pointer to pointer to arguments
// return: int                          0
//==============================================================================
int main (int argc, char *argv[])
{
 int res = EXIT_SUCCESS;

 // disable ctrl-C, ctrl-\ and ctrl-Z as it can leave floppy in unstable state
#ifdef WIN32
 // disables ctrl-C
 SetConsoleCtrlHandler(NULL, TRUE);
#else
 // disables ctrl-C, ctrl-\ and ctrl-Z
 signal(SIGINT,SIG_IGN);  // disable ctrl-C
 signal(SIGQUIT,SIG_IGN); // disable ctrl-backslash
 signal(SIGTSTP,SIG_IGN); // disable ctrl-Z
#endif 

 int exitstatus = 0;

 // initialise the functions module (before any console IO)
 functions_init();

 // check the operating system is the correct version
 if (check_os_version() == -1)
    return -1;

 // options initilisation
 options_init();

 // get the default home account directory.  This will typically be something
 // like c:\Program files\ubeedisk under windows and /home/username/.ubeedisk
 // under Unices if running from standard locations.
#ifdef WIN32
 int i;
 
 // get the path and executable filename
 if (GetModuleFileName(NULL, userhome, sizeof(userhome)) == 0)
    {
     printf(APPNAME": Unable to find the executable path\n");
     return -1;
    }

 // delete the executable name part and last '\' character
 i = strlen(userhome);
 while (userhome[--i] != SLASHCHAR)
    {}
 userhome[i] = 0;
 strcpy(userhome_path, userhome);
#else
 char *s;

 s = getenv("HOME");
 if (s == NULL)
    {
     printf(APPNAME": Unable to find the user's home path\n");
     return -1;
    }
 else
    {
     strcpy(userhome_path, s);
     snprintf(userhome, sizeof(userhome), "%s/."APPNAME, s);
    } 
#endif

 snprintf(userhome_confpath, sizeof(userhome_confpath), "%s%s",
 userhome, DIR_CONF);

 // process command line and initilisation options
 if (! exitstatus)
#ifdef WIN32 
    exitstatus = options_process(c_argc, c_argv);
#else    
    exitstatus = options_process(argc, argv);
#endif    

 // warn user if an old config file is being used
 if (exitstatus == 0 && strcasecmp(config_vers, "none") != 0 &&
 strcasecmp(config_file, "none") != 0 && strverscmp(config_vers, APPVER) < 0)
    printf(APPNAME": Warning - The configuration file is an older "
    "version: %s\n", config_vers);

 if (exitstatus != 0)
    res = EXIT_FAILURE;
 else
    {
     dsk_reportfunc_set(report, report_end);

     switch (disk.disk)
        {
         case UBEEDISK_COPY : // copy disks
            res = copy_disks();
            break;
         case UBEEDISK_DUMP : // reserved for disk dump copy
            printf(APPNAME": disk dump option not implemented.\n");
            res = -1;
            break;
         case UBEEDISK_GEOMETRY : // show detect format
            res = dump_geometry();
            break;
         case UBEEDISK_SCAN : // disk scan
            res = disk_scan();
            break;
         case UBEEDISK_FORMAT : // disk format
            res = disk_format();
            break;
         case UBEEDISK_SPEED : // disk speed test
            res = disk_speed();
            break;
         case UBEEDISK_CLEAN : // disk head clean
            res = disk_clean('c', 0);
            break;
        }

     res = (res == 0)? EXIT_SUCCESS:EXIT_FAILURE;
    }

 // de-initialise the functions module
 functions_deinit();

 close_files();

 return res;
}
