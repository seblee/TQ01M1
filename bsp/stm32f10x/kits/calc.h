#ifndef __CALC_H
#define __CALC_H

#include "stdint.h"

//循环移位
#define Ror(a, l, n) (a >> n) | (a << (l - n))
#define Rol(a, l, n) (a << n) | (a >> (l - n))

uint8_t checksum_u8(uint8_t *data_ptr, uint16_t data_num);
uint16_t checksum_u16(uint16_t *data_ptr, uint16_t data_num);
int16_t lim_min_max(int16_t min, int16_t max, int16_t data);
int16_t bin_search(uint16_t sSource[], uint16_t array_size, uint16_t key);
uint8_t xor_checksum(uint8_t *data_ptr, uint16_t data_num);
unsigned char BitCount_16(unsigned short n);
void quick(uint16_t *a, int16_t i, int16_t j); //?ì?ù??Dò

#endif //__CALC_H
