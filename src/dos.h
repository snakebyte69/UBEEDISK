/* DOS header */

#ifndef HEADER_DOS_H
#define HEADER_DOS_H

#pragma pack(push, 1)  // push current alignment, alignment to 1 byte boundary
typedef struct dos_header_t
{   
 uint8_t instr0;
 uint8_t instr1;
 uint8_t instr2;
 uint8_t oem_name[8];
 uint8_t filler[0x01fe - 11];
 uint16_t signature;
}dos_header_t;

typedef struct atari_header_t
{   
 uint8_t instr0;
 uint8_t instr1;
 uint8_t oem_name[6];
 uint8_t serial[3];
 uint8_t filler[0x01fe - 11];
 uint16_t checksum;
}atari_header_t;

typedef struct msx_header_t
{   
 uint8_t instr0;
 uint8_t instr1;
 uint8_t instr2;
 uint8_t oem_name[8];
 uint16_t msxdos1_cep;
 uint8_t msxdos2_volsign[6];
 uint8_t msxdos2_undel;
 uint32_t msxdos2_serial; 
 uint8_t reserved;
 uint16_t msxdos2_cep; 
 uint8_t filler[0x01fe - 27];
 uint16_t signature;
}msx_header_t;

typedef struct bpb_t
{
 uint8_t filler[11];
 uint16_t bytes_sect;
 uint8_t sect_clust; 
 uint16_t res_sect;
 uint8_t fats;
 uint16_t root_dir_entries;
 uint16_t total_sectors;
 uint8_t media_desc;
 uint16_t sect_per_fat;
 uint16_t sect_track; 
 uint16_t number_sides;
 uint16_t number_hidden_sectors;
 uint8_t bstrap_part_info[480];
}bpb_t;

typedef union dos_bootstrap_u
{   
 dos_header_t dos;
 atari_header_t atari;
 msx_header_t msx; 
 bpb_t bpb;
}dos_bootstrap_u;
#pragma pack(pop)      // restore original alignment from stack

int dos_detect_format (void);

#endif     /* HEADER_DOS_H */
