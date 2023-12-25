/* functions header */

#ifndef HEADER_FUNCTIONS_H
#define HEADER_FUNCTIONS_H

typedef struct sup_args_t
{
 const char *name;
 int value;
}sup_args_t;

#define le16_to_host little_endian_16
#define host_to_le16 little_endian_16
#define leu16_to_host little_endian_u16
#define host_to_leu16 little_endian_u16
#define le32_to_host little_endian_32
#define host_to_le32 little_endian_32
#define leu32_to_host little_endian_u32
#define host_to_leu32 little_endian_u32
#define le64_to_host little_endian_64
#define host_to_le64 little_endian_64
#define leu64_to_host little_endian_u64
#define host_to_leu64 little_endian_u64

#define be16_to_host big_endian_16
#define host_to_be16 big_endian_16
#define beu16_to_host big_endian_u16
#define host_to_beu16 big_endian_u16
#define be32_to_host big_endian_32
#define host_to_be32 big_endian_32
#define beu32_to_host big_endian_u32
#define host_to_beu32 big_endian_u32
#define be64_to_host big_endian_64
#define host_to_be64 big_endian_64
#define beu64_to_host big_endian_u64
#define host_to_beu64 big_endian_u64

int IsBigEndian (void);
void swap_endian (unsigned char *na, int size);

int16_t little_endian_16 (int16_t n);
uint16_t little_endian_u16 (uint16_t n);
int32_t little_endian_32 (int32_t n);
uint32_t little_endian_u32 (uint32_t n);
int64_t little_endian_64 (int64_t n);
uint64_t little_endian_u64 (uint64_t n);

int16_t big_endian_16 (int16_t n);
uint16_t big_endian_u16 (uint16_t n);
int32_t big_endian_32 (int32_t n);
uint32_t big_endian_u32 (uint32_t n);
int64_t big_endian_64 (int64_t n);
uint64_t big_endian_u64 (uint64_t n);

int get_key (void);
#ifdef WIN32
char *strcasestr (char *haystack, char *needle);
int strverscmp (const char *s1, const char *s2);
#else
extern int printfx (char * fmt, ...)
                __attribute__ ((format (printf, 1, 2)));
char *fgetsx(char *s, int size, FILE *stream);
#endif
void sleep_ms (int ms);
uint64_t time_get_ms (void);
void toupper_string (char *dest, char *src);
void tolower_string (char *dest, char *src);
int string_search (char *strg_array[], char *strg_find);
int string_case_search (char *strg_array[], char *strg_find);
int string_struct_search (sup_args_t *args, char *strg_find);
int string_struct_search_i (sup_args_t *args, int value);
char *memstr (void *m, int n, char *s);
int get_integer_value (char *s);
char *get_next_parameter (char *s, int delimiter, char *sps, int *spi, int maxlen);
int get_int_arguments (char *parms, int *p, int max_ent, int max_val);
int get_colon_arguments (char *parms, int *p, int max_ent, int max_val);
int get_gap_colon_arguments (char *parms, int *p, int max_ent, int max_val);
void file_name_part (char *pathfile, char *filename);
void create_md5 (char *filename, char *md5);
int check_os_version (void);
void reset_drive (char *xtype, char *xfile);
int get_drive_type (char *xtype, char *xfile, DSK_PDRIVER xdrive);
void functions_init (void);
void functions_deinit (void);

#endif     /* HEADER_FUNCTIONS_H */
