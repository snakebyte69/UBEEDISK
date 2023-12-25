/* ubeedisk header */

#ifndef HEADER_UBEEDISK_H
#define HEADER_UBEEDISK_H

#include <libdsk.h>

void report_dg (DSK_GEOMETRY *dg);
void close_files (void);
dsk_err_t override_values (void);
int read_sector_id (dsk_pcyl_t cyl, dsk_phead_t head, 
int *xhead, int *xsecsize);
int file_disk_description (char *s);
void process_disk_description (char *s);
int scan_option (char *parms);
void close_reopen_pc_floppy_input_drive (void);

#ifdef WIN32
#define SLASHCHAR '\\'
#define SLASHCHAR_STR "\\"
#define SLASHCHAR_OTHER '/'
#define DIR_CONF "\\"
#else
#define SLASHCHAR '/'
#define SLASHCHAR_STR "/"
#define SLASHCHAR_OTHER '\\'
#define DIR_CONF "/"
#endif

#ifndef WIN32
#undef printf
#define printf(...)  printfx(__VA_ARGS__)
#define fgets fgetsx
#endif

#define SSIZE1 512
#define INFO_SIZE 100000
#define PSKEW_SIZE 256

#define DESC_LINES 100
#define DESC_CHARS 100

#define ESC 27

enum
{
 UBEEDISK_COPY,
 UBEEDISK_DUMP,
 UBEEDISK_GEOMETRY,
 UBEEDISK_SCAN,
 UBEEDISK_FORMAT,
 UBEEDISK_SPEED,
 UBEEDISK_CLEAN
};

typedef struct ubd_t
{
 int system;
 char sysname[SSIZE1];
}ubd_t;

typedef struct disk_t
{
 int append_error;
 int iautorate;
 int oautorate;
 int cacher;
 int cachew;
 int count;
 int detect;
 int disk;
 int enter_desc;
 int erase;
 int write_error_count;
 int finish;
 int first_read;
 int force;
 int forceside;
 int head_s;
 int head_f;
 int idrive_type;
 int ignore_errors;
 int info_file;
 int iside;
 int iside_not_support;
 int idstep;
 int idstep_used;
 int log;
 int mediadesc;
 int nofill;
 int noformat;
 int odrive_type;
 int odstep;
 int odstep_used;
 int oside;
 int oside_not_support;
 int overwrite;
 int retries_l1;
 int retries_l2;
 int start;
 int sfmode;
 int support_xread;
 int support_xwrite;
 int gap_set[8];
 int fd_workaround1;
 int fd_workaround2;
 int verbose;
 int unattended;
 int unattended_retry_abort_max;
 int unattended_retry_sector_max;
 int unattended_seeked_max;
 char format[1000];
 char ifile[1000];
 char incomp[1000];
 char itype[1000];
 char ofile[1000];
 char otype[1000];
 char outcomp[1000];
 char signature[1000];
 int pskew0[PSKEW_SIZE];
 int pskew1[PSKEW_SIZE];
 int pskew0_opt;
 int pskew1_opt;
}disk_t;

typedef struct dg_opts_t
{
 int sidedness;
 int cylinders;
 int heads;
 int sectors;
 int secbase;
 int secsize;
 int idatarate;
 int odatarate;
 int rwgap;    
 int fmtgap;
 int ifm;
 int ofm; 
 int nomulti;
 int noskip;
 int sideoffs; 
 int side1as0;
 int secbase1s;
 int secbase2s;
 int secbase2c;
 int skew_val;
 int skew_ofs;
 int special;
 int attributes;
 char format_name[1000];
 char format_desc[1000];
}dg_opts_t;

typedef struct xdg_t
{
 int dg_sideoffs;
 int dg_side1as0;
 int dg_secbase1s;
 int dg_secbase2s;
 int dg_secbase2c;
 int dg_idatarate;
 int dg_odatarate;
 int dg_ifm;
 int dg_ofm; 
 int dg_skew_val;
 int dg_skew_ofs;
 int dg_special;
 int dg_attributes;

 char dg_format_name[1000];
 char dg_format_desc[1000];

 int (*ssr_cb)(uint8_t *, int, int, int); // read buffered track call-back
 int (*rsi_cb)(int, int, int *, int *); // read sector id call-back
 int (*ssd_cb)(int, int, int); // set special disk call-back
 int (*cif_cb)(int); // create info file call-back
 int (*pss_cb)(int, int, int); // physical sector skewing call-back

 int *dg_pskew0;
 int *dg_pskew1;
}xdg_t;

typedef struct ext_type_t
{
 char *ext;
 char *type;
}ext_type_t;

typedef struct info_t
{
 int cyl_last;
 int head_last;
 int pos_trk;
 int pos_sec;
 int buf[INFO_SIZE];
}info_t;

typedef struct sup_t
{
 int psecid;
 int ptrackids;
 int pformat;
 int xread;
 int xwrite;
 char remote_interface[101];
 char version[101];
}sup_t;

// gap flags
#define FDC_DG_FMTGAP 0x3ffe
#define FDC_AUTO_GAP 0x3fff
#define GAP4 6

// format directives
enum
{
 FDC_FMT_GAP1,
 FDC_FMT_GAP2,
 FDC_FMT_GAP3,
 FDC_FMT_GAP4,
 FDC_FMT_GAP5, 

 FDC_FMT_SYNC1,
 FDC_FMT_SYNC2,
 FDC_FMT_SYNC3,

 FDC_FMT_TRACK,
 FDC_FMT_SIDE,
 FDC_FMT_SECTOR,
 FDC_FMT_LENGTH,
 FDC_FMT_DATA,

 FDC_FMT_F5,
 FDC_FMT_F6,
 FDC_FMT_IM,
 FDC_FMT_IDAM,
 FDC_FMT_DAM, 
 FDC_FMT_CRC
};

#pragma pack(push, 1)  // push current alignment, alignment to 1 byte boundary
typedef struct fdc_format_data_t
{ 
 uint8_t id;
 uint8_t data;
 int16_t count;
}fdc_format_data_t;

typedef struct fdc_format_gap_t
{
 int16_t gap0;
 int16_t gap1;
 int16_t gap2;
 int16_t gap3;
}fdc_format_gap_t;

#pragma pack(pop)      // restore original alignment from stack

#endif     /* HEADER_UBEEDISK_H */
