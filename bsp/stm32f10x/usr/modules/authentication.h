#ifndef __AHTHEN_H__
#define __AHTHEN_H__
#include "stdint.h"

typedef enum
{
		GUEST = 1,
		OPERATOR,
		ENGINEER,
		ADMIN		
}em_usr_name;

uint16_t authen_init(void);		
uint16_t authen_verify(em_usr_name user_name,uint8_t* password_ptr, uint8_t buf_len);
uint16_t authen_revise(em_usr_name user_name,uint8_t* password_ptr, uint8_t buf_len);
void authen_expire_set(void);
void authen_expire_cd(void);


#endif  /*__AHTHEN_H__*/
