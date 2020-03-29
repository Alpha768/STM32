
#include "Include.h"

#define POLY 0x04C11DB7L
#define UPOLY 0xEDB88320L

unsigned long get_sum_poly( unsigned char top_byte )
{
    /**//// sum all the polys at various offsets
	int j;
    unsigned long sum_poly = top_byte << 24;
    for ( j = 0; j < 8; ++ j )
    {
        /**//// check the top bit
        if ( ( sum_poly >> 31 ) != 0 )
        {
            /**//// TODO : understand why '< <' first
            sum_poly = ( sum_poly << 1 ) ^ POLY;
        }
        else
        {
            sum_poly <<= 1;
        }
    }
	
    return sum_poly;
}

unsigned long get_sum_upoly( unsigned char top_byte )
{
	unsigned long sum_upoly;
	int j;
	sum_upoly = top_byte;
	for (j = 8; j > 0; j--)
	{
		if (sum_upoly & 1)
			sum_upoly = (sum_upoly >> 1) ^ UPOLY;
		else
			sum_upoly >>= 1;
  
	}
  return sum_upoly;
}

void create_table_poly( unsigned long *table )
{
	int i;
    for ( i = 0; i < 256; ++ i )
    {
        table[i] = get_sum_poly( (unsigned char) i );
    }
}

void create_table_upoly( unsigned long *table )
{
	int i;
    for ( i = 0; i < 256; ++ i )
    {
        table[i] = get_sum_upoly( (unsigned char) i );
    }
}

// the table
static   unsigned long table[256];
static unsigned char f_loaded=0;

unsigned long CRC32(unsigned char *ptr, unsigned long len)
{
	unsigned long crc = 0;
	unsigned long i=0;
	
    // create the table
	  if(f_loaded==0)
		{
			create_table_poly( table );
			f_loaded=1;
		}
	for ( i = 0; i < len; i++ )
    {
        crc = ( crc << 8 ) ^ table[ ( crc >> 24 ) ^ ptr[i] ];
    }
	return crc;
}

/*
unsigned long CRC32(unsigned char *ptr, unsigned long len)
{
	unsigned long crc = 0xffffffff;
	unsigned long i=0;
	// the table
    unsigned long table[256];
    // create the table
    create_table_upoly( table );
	for ( i = 0; i < len; i++ )
    {
        crc = ( crc >> 8 ) ^ table[ ( crc &0xff ) ^ ptr[i] ];
    }
	crc^= 0xffffffff;
	return crc;
}
*/

