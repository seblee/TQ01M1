#ifndef __CYC_BUFF_H
#define	__CYC_BUFF_H

#include "sys_def.h"
#pragma pack (1)

 typedef struct
 {
	  uint16_t startpiont;
	  uint16_t endpiont;
	  uint16_t cnt;
 }record_pt_st;
 

 typedef struct
 {
	  record_pt_st pt_inst;
		uint16_t depth;
		uint16_t block_size;
		uint16_t eeprom_buff_base_addr;
		uint32_t size;
 }cyc_buff_st;
 

#pragma pack ()
 
 
 
uint32_t init_cycbuff(cyc_buff_st* buff_pt,uint16_t block_size, uint16_t dep,uint16_t eeprom_addr);
 
void add_cycbuff(cyc_buff_st* buff_pt,uint8_t* buff_data );
void add_n_cycbuff(cyc_buff_st* buff_st_ptr,uint8_t* buff_data ,uint16_t n);

 
uint16_t quiry_cycbuff(cyc_buff_st* buff_st_ptr,uint16_t offset,uint8_t cnt,uint8_t* buff_data);
 

uint8_t clear_cycbuff_log(cyc_buff_st* buff_st_ptr);
; 
 
 
#endif
 
 
 
 
 
