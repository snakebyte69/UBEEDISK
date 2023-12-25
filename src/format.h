/* format header */

#ifndef HEADER_FORMAT_H
#define HEADER_FORMAT_H

#define SKEW_VAL_DEF 2
#define SKEW_OFS_DEF 1

#define ATTR_VAR_TRACK_SIZE (1 << 0)
#define ATTR_CACHER_OFF     (2 << 0)
#define ATTR_CACHEW_OFF     (4 << 0)

enum
{
 det_off,
 det_mbee,
 det_mbee3,
 det_mbee5,
 det_applix,
 det_dos,
 det_atari,
 det_msxdos,
 det_fm,
 det_various
};

enum
{
 dos_none,
 dos_ms,
 dos_atari,
 dos_msx,
};

typedef struct disk_format_t
{
 char name[100];     // format name
 char desc[100];     // format description
 int sidedness;      // how to handle multisided discs
 int cylinders;      // number of cylinders
 int heads;          // number of heads
 int sectors;        // sectors per track
 int secbase;        // first physical sector number
 int secsize;        // sector size in bytes
 int datarate;       // data rate
 int rwgap;          // read/write gap length
 int fmtgap;         // format gap length
 int fm;             // FM mode

 int nomulti;        // no multitrack
 int noskip;         // no skipping deleted data
 int sideoffs;       // side offset value
 int side1as0;       // side1as0 issue
 int secbase2_c;     // 2nd sector base cylinder change
 int secbase2_s;     // 2nd sector base first sector
 int skew_val;       // sector skew value
 int skew_ofs;       // offset to apply to sector base
 int special;        // special flags
 int attributes;     // attribute flags
 
 int (*ssr_cb)(uint8_t *, int, int, int); // read buffered track call-back
 int (*rsi_cb)(int, int, int *, int *); // read sector id call-back
 int (*ssd_cb)(int, int, int); // set special disk call-back
 int (*cif_cb)(int); // create info file call-back
 int (*pss_cb)(int, int, int); // physical sector skewing call-back

 int *pskew0;        // pointer to physical skew table data for side 0
 int *pskew1;        // pointer to physical skew table data for side 1

 char gap[100];      // option GAP values for format
}disk_format_t;

dsk_err_t read_sector_ids (int cyl, int head, int secsize, int *sizeok,
                           int *size, dsk_psect_t *count,
                           int *txhx_sec_f, int *txhx_sec_l);
dsk_err_t read_physical_sector (dsk_pcyl_t cyl, dsk_pcyl_t xcyl,
                                dsk_phead_t head, dsk_phead_t xhead,
                                int psect, uint8_t *buffer);
int format_set_geometry (disk_format_t *disk_format);
void format_list_formats (void);
void format_list_types (void);
int format_detect_disk (void);
int format_set (char *format_id, int report);
int format_detect_select (char *format_id);

#endif     /* HEADER_FORMAT_H */
