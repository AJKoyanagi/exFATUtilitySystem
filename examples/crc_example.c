#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>

int main()
{
    // this routine shows how to use crc32() to calculate a checksum
    // on a block of memory.
    unsigned char *p = (unsigned char*) malloc(1000);

    u_int32_t initialCrc = crc32( 0,p,1000);
    printf("the initial crc is %x\n",initialCrc);

    // change a byte
    *(p+100) = 0xff;
    u_int32_t changedCrc = crc32 (0,p,1000);
    printf("the crc after changing a byte is %x\n",changedCrc);
    

    return 0;
}
