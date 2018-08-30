#include "stdint.h"
#include "calc.h"

uint8_t checksum_u8(uint8_t* data_ptr, uint16_t data_num)
{
		uint16_t 	i;
		uint8_t 	checksum_res;
		checksum_res = *data_ptr;
		for(i=1;i<data_num;i++)
		{
				checksum_res ^= *(data_ptr+i);
		}
		return checksum_res;
}

uint16_t checksum_u16(uint16_t* data_ptr, uint16_t data_num)
{
		uint16_t 	i;
		uint16_t 	checksum_res;
		checksum_res = *data_ptr;
		for(i=1;i<data_num;i++)
		{
				checksum_res ^= *(data_ptr+i);
		}
		return checksum_res;
}

uint8_t xor_checksum(uint8_t* data_ptr, uint16_t data_num)
{
		uint16_t i;
		uint8_t 	checksum_res;
		checksum_res = 0;
		for(i=0;i<data_num;i++)
		{
				checksum_res ^= *(data_ptr+i);
		}
		return checksum_res;
}

int16_t lim_min_max(int16_t min, int16_t max, int16_t data)
{
		if(data <= min)
		{
				return min;
		}
		else if(data >= max)
		{
				return max;
		}
		else
		{
				return data;
		}
}

int16_t bin_search(uint16_t *a_ptr, uint16_t array_size, uint16_t key)  
{     
    int low = 0, high = array_size - 1, mid;  
		
		if((key < *(a_ptr))||(key > *(a_ptr+array_size)))
		{
				return -1;
		}	
      
    while (low <= high)  
    {         
        mid = (low + high) / 2; 
          
        if ((*(a_ptr+mid) <= key)&&(*(a_ptr+mid+1) > key)&&((mid+1)<array_size))
				{
            return mid;   
				}
				else
				{
						if (*(a_ptr+mid) > key)      
						{
								high = mid - 1;
						}
						else
						{
								low = mid + 1;
						}
				}
    }
    return -1;    
}

unsigned char BitCount_16(unsigned short n)
{
    unsigned char c = 0 ; //??¨ºy?¡Â
    for (c = 0; n; n >>= 1) //¨°???
        c += n & 1 ; //¦Ì¡À?¡ã??¨º?1¡ê??¨®1
    return c ;
}

void quick(uint16_t *a,int16_t i,int16_t j)        //?¨¬?¨´??D¨°
{
    int16_t m,n,temp;
    uint16_t k;
    m=i;
    n=j;
    k=a[(i+j)/2]; /*??¨¨?¦Ì?2???*/
    do
    {
        while(a[m]<k&&m<j)
        {
            m++;    /* ¡ä¨®¡Á¨®¦Ì?¨®¨°?¨°¡À¨¨k¡ä¨®¦Ì??a??*/
        }
        while(a[n]>k&&n>i)
        {
            n--;    /* ¡ä¨®¨®¨°¦Ì?¡Á¨®?¨°¡À¨¨kD?¦Ì??a??*/
        }
        if(m<=n)   /*¨¨??¨°¦Ì??¨°?¨²¡Á?¨¬??t¡ê??¨°????*/
        {
            temp=a[m];
            a[m]=a[n];
            a[n]=temp;
            m++;
            n--;
        }
    }
    while(m<=n);
    if(m<j)
    {
        quick(a,m,j);    /*??¨®?¦ÌY1¨¦*/
    }
    if(n>i)
    {
        quick(a,i,n);
    }
}

