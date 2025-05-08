#include "extfat.h"
#include "routines.h"

// comapares main_boot with the backup boot checksum values and makes sure they are the same size
// this essentially makes sure that the verify flag implementation works as expected
void compareBoot(void* MB, void* BB, int bytespersector)
{
    uint32_t mbrChecksum = 0;
    uint32_t bbrChecksum = 0;
    

    mbrChecksum = BootChecksum((uint8_t*) MB, (short) bytespersector);
    bbrChecksum = BootChecksum((uint8_t*) BB, (short) bytespersector);
    printf("Checksum  (MB) %x (BBR) %x\n",mbrChecksum,bbrChecksum);

    if(mbrChecksum == bbrChecksum)
    {
        printf("Checksum values between Main_Boot and Backup_Boot are the same\n");
    }

    else
    {
        printf("Checksum values between Main_Boot and Backup_Boot are not the same\n");
    }           
}
