/***********************************************************************************************
Íâ²¿´æ´¢Æ÷ Ñ­»·´æ´¢
************************************************************************************************/


#include "cyc_buff.h"
#include "i2c_bsp.h"

uint32_t init_cycbuff(cyc_buff_st* buff_st_ptr,uint16_t block_size, uint16_t depth,uint16_t eeprom_addr)
{
		uint32_t size;
	
		buff_st_ptr->block_size = block_size;
		buff_st_ptr->eeprom_buff_base_addr = eeprom_addr;
		buff_st_ptr->depth =depth-1;
	    size = sizeof(record_pt_st)+block_size*depth;
	    buff_st_ptr->size = size;
	
		return(size);
	
}
static uint16_t cacl_pt(cyc_buff_st* buff_st_ptr)
{
   uint16_t data_addr;
	
		//read data_pt
		I2C_EE_BufRead((uint8_t*)&buff_st_ptr->pt_inst,buff_st_ptr->eeprom_buff_base_addr,sizeof(record_pt_st));
	
	
	 buff_st_ptr->pt_inst.cnt++;
	 if( buff_st_ptr->pt_inst.cnt > buff_st_ptr->depth)
	 {
	 	 buff_st_ptr->pt_inst.cnt = buff_st_ptr->depth+1;//
	 	 buff_st_ptr->pt_inst.startpiont++;
		 if( buff_st_ptr->pt_inst.startpiont >  buff_st_ptr->depth)
		 {
				 buff_st_ptr->pt_inst.startpiont = 0;
		 }
	 }

	   buff_st_ptr->pt_inst.endpiont++;
	 if(  buff_st_ptr->pt_inst.endpiont > buff_st_ptr->depth)
	 {
	 	  buff_st_ptr->pt_inst.endpiont = 0; 
	 }
	 //save addr pt to EEPROM
	
		I2C_EE_BufWrite((uint8_t*)&buff_st_ptr->pt_inst,buff_st_ptr->eeprom_buff_base_addr,sizeof(record_pt_st));
	 //calc  data addr
   data_addr = buff_st_ptr->eeprom_buff_base_addr + sizeof(record_pt_st) + buff_st_ptr->pt_inst.endpiont * buff_st_ptr->block_size;
	 return(data_addr);
}

static uint16_t cacl_pt_n(cyc_buff_st* buff_st_ptr,uint16_t n,uint16_t* write_n_ptr)
{
  		 uint16_t data_addr,i,cnt;
	
		//read data_pt
		data_addr=cacl_pt(buff_st_ptr);
		
		I2C_EE_BufRead((uint8_t*)&buff_st_ptr->pt_inst,buff_st_ptr->eeprom_buff_base_addr,sizeof(record_pt_st));

		cnt = 1;
		for(i = 0;i < (n-1);i++)
		{
			 buff_st_ptr->pt_inst.cnt++;
			 if( buff_st_ptr->pt_inst.cnt > buff_st_ptr->depth)
			 {
			 	 buff_st_ptr->pt_inst.cnt = buff_st_ptr->depth;//
			 	 buff_st_ptr->pt_inst.startpiont++;
				 if( buff_st_ptr->pt_inst.startpiont >  buff_st_ptr->depth)
				 {
					  buff_st_ptr->pt_inst.startpiont = 0;
				 }
			 }
			 buff_st_ptr->pt_inst.endpiont++;
			 cnt++; 
			 if(  buff_st_ptr->pt_inst.endpiont >= buff_st_ptr->depth)
			 {
			 	  buff_st_ptr->pt_inst.endpiont = buff_st_ptr->depth; 
				  *write_n_ptr = cnt;
				  break;
			 }
			 
		}
	 //save addr pt to EEPROM
	
		I2C_EE_BufWrite((uint8_t*)&buff_st_ptr->pt_inst,buff_st_ptr->eeprom_buff_base_addr,sizeof(record_pt_st));
	 //calc  data addr
  
	 
	 return(data_addr);
}




void add_cycbuff(cyc_buff_st* buff_st_ptr,uint8_t* buff_data )
{
		uint16_t data_addr;
	
	
		data_addr=cacl_pt(buff_st_ptr);
		//save event data to EEPROM;
		I2C_EE_BufWrite(buff_data,data_addr,buff_st_ptr->block_size);
}

void add_n_cycbuff(cyc_buff_st* buff_st_ptr,uint8_t* buff_data ,uint16_t n)
{
		uint16_t data_addr,writ_n1,write_n2;
	
	
		data_addr=cacl_pt_n(buff_st_ptr,n,&writ_n1);
		I2C_EE_BufWrite(buff_data,data_addr,buff_st_ptr->block_size*writ_n1);
		
		if(writ_n1 < n)
		{
			data_addr=cacl_pt_n(buff_st_ptr,(n-writ_n1),&write_n2);
			I2C_EE_BufWrite((buff_data+buff_st_ptr->block_size*writ_n1),data_addr,buff_st_ptr->block_size*write_n2);	
		}
		
}

uint16_t quiry_cycbuff(cyc_buff_st* buff_st_ptr,uint16_t offset,uint8_t cnt,uint8_t* buff_data)
{
		uint8_t read_cnt;
		uint16_t data_addr;
	
		read_cnt = 0;
		
		
	if(buff_st_ptr->pt_inst.cnt < offset)
	{
		return(0);
	}
	if((buff_st_ptr->pt_inst.cnt-offset) < cnt)
	{
		cnt = buff_st_ptr->pt_inst.cnt - offset;
	}
	//??????
	if(buff_st_ptr->pt_inst.endpiont < offset)
	{
		buff_st_ptr->pt_inst.endpiont = offset - buff_st_ptr->pt_inst.endpiont;
		buff_st_ptr->pt_inst.endpiont = buff_st_ptr->depth + 1 - buff_st_ptr->pt_inst.endpiont;
	}
	else
	{
		buff_st_ptr->pt_inst.endpiont = buff_st_ptr->pt_inst.endpiont-offset;
	}
	offset=0;
	

	while(cnt--)
	{
		data_addr = buff_st_ptr->eeprom_buff_base_addr+sizeof(record_pt_st);
		if(buff_st_ptr->pt_inst.endpiont == 0xFFFF)//????
		buff_st_ptr->pt_inst.endpiont =buff_st_ptr->depth;
		data_addr += buff_st_ptr->pt_inst.endpiont *buff_st_ptr->block_size;
	  //rt_kprintf("\nget_record_data: EndPiont=%d StartPiont=%d CNT=%d \n",pt.EndPiont,pt.StartPiont,pt.CNT);
		I2C_EE_BufRead((uint8_t*)(buff_data+offset),data_addr,buff_st_ptr->block_size);
		offset += buff_st_ptr->block_size;
		read_cnt++;
		buff_st_ptr->pt_inst.endpiont--;
//		rt_kprintf("\nget_record_data:\n");
//		for(i=offset;i<offset+len;i++)
//		{
//			rt_kprintf(" %x",*(Data+i));
//		}
	
	}
	return(read_cnt);
	
}

uint8_t clear_cycbuff_log(cyc_buff_st* buff_st_ptr)
{
		buff_st_ptr->pt_inst.endpiont = 0 ;
		buff_st_ptr->pt_inst.startpiont = 0;
		buff_st_ptr->pt_inst.cnt = 0;
		
		return(I2C_EE_BufWrite((uint8_t*)&buff_st_ptr->pt_inst,buff_st_ptr->eeprom_buff_base_addr,sizeof(record_pt_st)));
	
}



