#ifndef __FIFO_H
#define	__FIFO_H
#include "sys_def.h"
typedef struct fifo16_cb{
	uint16_t* buffer_ptr;
	uint16_t* bhead_ptr;
	uint16_t* btail_ptr;
	uint16_t	block_size;
	uint16_t	depth;
	uint16_t	length;	
}fifo16_cb_td;

typedef struct fifo8_cb{
	uint8_t* buffer_ptr;
	uint8_t* bhead_ptr;
	uint8_t* btail_ptr;
	uint16_t	block_size;
	uint16_t	depth;
	uint16_t	length;	
}fifo8_cb_td;

uint8_t fifo16_init(fifo16_cb_td* fifo16_cb_ptr, uint16_t block_size, uint16_t depth);
uint8_t	fifo16_push(fifo16_cb_td* fifo_cb, uint16_t* src_addr);
uint8_t	fifo16_pop(fifo16_cb_td* fifo_cb, uint16_t* dest_addr);
void fifo16_reset(fifo16_cb_td* fifo_cb);
uint8_t	is_fifo16_empty(fifo16_cb_td* fifo_cb);
uint8_t	is_fifo16_full(fifo16_cb_td* fifo_cb);
uint16_t	get_fifo16_length(fifo16_cb_td* fifo_cb);

uint8_t fifo8_init(fifo8_cb_td* fifo16_cb_ptr, uint16_t block_size, uint16_t depth);
uint8_t	fifo8_push(fifo8_cb_td* fifo_cb, uint8_t* src_addr);
uint8_t	fifo8_pop(fifo8_cb_td* fifo_cb, uint8_t* dest_addr);
void fifo8_reset(fifo8_cb_td* fifo_cb);
uint16_t	get_fifo8_length(fifo8_cb_td* fifo_cb);
uint8_t	is_fifo8_empty(fifo8_cb_td* fifo_cb);
uint8_t	is_fifo8_full(fifo8_cb_td* fifo_cb);

#endif //__FIFO_H


