/**
  ******************************************************************************
  * @file    HASH/SHA256/main.c
  * @author  MCD Application Team
  * @version V2.0.6
  * @date    25-June-2013
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <rtthread.h>
#include <components.h>
#include "sys_conf.h"
#include "local_status.h"
#include "crypto.h"
#include "i2c_bsp.h"
#include "authentication.h"

#define GUEST_PW_STR			"111111"
#define OPERATOR_PW_STR		"222222"
#define ENGINEER_PW_STR		"333333"
#define ADMIN_PW_STR			"444444"

#define AUTHEN_EXPIRE_TIME	600

/**
  * @brief  SHA256 HASH digest compute example.
  * @param  InputMessage: pointer to input message to be hashed.
  * @param  InputMessageLength: input data message length in byte.
  * @param  MessageDigest: pointer to output parameter that will handle message digest
  * @param  MessageDigestLength: pointer to output digest length.
  * @retval error status: can be HASH_SUCCESS if success or one of
  *         HASH_ERR_BAD_PARAMETER, HASH_ERR_BAD_CONTEXT,
  *         HASH_ERR_BAD_OPERATION if error occured.
  */
static int32_t STM32_SHA256_HASH_DigestCompute(uint8_t* InputMessage, uint32_t InputMessageLength,
                                  uint8_t *MessageDigest, int32_t* MessageDigestLength)
{
  SHA256ctx_stt P_pSHA256ctx;
  uint32_t error_status = HASH_SUCCESS;
	
	Crypto_DeInit();

  /* Set the size of the desired hash digest */
  P_pSHA256ctx.mTagSize = CRL_SHA256_SIZE;

  /* Set flag field to default value */
  P_pSHA256ctx.mFlags = E_HASH_DEFAULT;

  error_status = SHA256_Init(&P_pSHA256ctx);

  /* check for initialization errors */
  if (error_status == HASH_SUCCESS)
  {
    /* Add data to be hashed */
    error_status = SHA256_Append(&P_pSHA256ctx,
                                 InputMessage,
                                 InputMessageLength);

    if (error_status == HASH_SUCCESS)
    {
      /* retrieve */
      error_status = SHA256_Finish(&P_pSHA256ctx, MessageDigest, MessageDigestLength);
    }
  }

  return error_status;
}




/**
  * @brief  Compares two buffers.
  * @param  pBuffer, pBuffer1: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer identical to pBuffer1
  *         FAILED: pBuffer differs from pBuffer1
  */
static uint32_t Buffercmp(const uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer != *pBuffer1)
    {
      return 0;
    }

    pBuffer++;
    pBuffer1++;
  }

  return 1;
}

//static void read_password(uint8_t pn)
//{
//		uint8_t rd_buf[32];
//		uint8_t i;
//	
//		I2C_EE_BufRead(rd_buf,(USR_PW_EE_BASE + (32<<pn)),32);
//		rt_kprintf("pwd no.%d\n",pn);
//		for(i=0;i<32;i++)
//		{
//				rt_kprintf("%x",rd_buf[i]);
//		}
//}

uint16_t authen_init(void)
{
		char ibuf_0[] = GUEST_PW_STR;
		char ibuf_1[] = OPERATOR_PW_STR;
		char ibuf_2[] = ENGINEER_PW_STR;
		char ibuf_3[] = ADMIN_PW_STR;
		uint8_t *str_ptr;
		uint8_t obuf[32];
		int32_t obuf_len;	
		uint8_t i;
		uint8_t size;			
		Crypto_DeInit();
		for(i=0;i<4;i++)
		{
				if(i == 0)
				{
						str_ptr = (uint8_t *)ibuf_0;
						size = sizeof(ibuf_0) - 1;
				}
				else if(i == 1)
				{
						str_ptr = (uint8_t *)ibuf_1;
						size = sizeof(ibuf_1) - 1;
				}
				else if(i == 2)
				{
						str_ptr = (uint8_t *)ibuf_2;
						size = sizeof(ibuf_2) - 1;						
				}		
				else if(i == 3)
				{
						str_ptr = (uint8_t *)ibuf_3;
						size = sizeof(ibuf_3) - 1;
				}		
				else 
				{
						return 0;
				}			
				STM32_SHA256_HASH_DigestCompute(str_ptr,size,obuf,&obuf_len);	
				I2C_EE_BufWrite(obuf,(USR_PW_EE_BASE+(32*i)),32);
		}
		return 1;		
}

uint16_t authen_verify(em_usr_name user_name,uint8_t* password_ptr, uint8_t buf_len)
{
		extern sys_reg_st	g_sys; 	
		uint8_t obuf[32];
		uint8_t pbuf[32];
		int32_t obuf_len;	
		Crypto_DeInit();
		STM32_SHA256_HASH_DigestCompute(password_ptr,buf_len,obuf,&obuf_len);
		
		I2C_EE_BufRead(pbuf,(USR_PW_EE_BASE + (32*(user_name-1))),obuf_len);
		
		if(Buffercmp(obuf,pbuf,obuf_len) == 1)
		{				
				g_sys.status.general.permission_level = user_name;
				rt_kprintf("permission over write\n");
				return 1;
		}
		else
		{
				g_sys.status.general.permission_level = 0;
				rt_kprintf("permission attemp fail\n");
				return 0;
		}				
}

uint16_t authen_revise(em_usr_name user_name,uint8_t* password_ptr, uint8_t buf_len)
{
		uint8_t obuf[32];
		int32_t obuf_len;	
		STM32_SHA256_HASH_DigestCompute(password_ptr,buf_len,obuf,&obuf_len);	
		I2C_EE_BufWrite(obuf,(USR_PW_EE_BASE+(32*(user_name-1))),32);
		return 1;
}

void authen_expire_set(void)
{
		extern local_reg_st 	l_sys;
		l_sys.authen_cd = AUTHEN_EXPIRE_TIME;
		
}

void authen_expire_cd(void)
{
		extern sys_reg_st	g_sys;
		extern local_reg_st 	l_sys;
		if(l_sys.authen_cd > 0)
		{
				l_sys.authen_cd--;
				if(l_sys.authen_cd == 0)
				{
						g_sys.status.general.permission_level = 0;
				}
				else
				{
						g_sys.status.general.permission_level = g_sys.status.general.permission_level;
				}
		}
		else
		{
				g_sys.status.general.permission_level = 0;
		}
}

static void list_ao_regs(uint16_t type)
{
		extern local_reg_st l_sys;
		uint16_t i;
		for(i=0;i<AO_MAX_CNT;i++)
		{
				rt_kprintf("%d:%x\n",i,l_sys.ao_list[i][type]);
		}
		
}

FINSH_FUNCTION_EXPORT(authen_verify, verify password.);
FINSH_FUNCTION_EXPORT(authen_revise, change password.);
FINSH_FUNCTION_EXPORT(list_ao_regs, list_aout_regs.);


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
