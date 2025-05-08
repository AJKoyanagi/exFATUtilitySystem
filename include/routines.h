#include <stdint.h>

// perhaps typedefs would be a better solution?

#define UInt32 uint32_t

#define UCHAR  uint8_t
#define USHORT uint16_t

UInt32 BootChecksum
(
    UCHAR  * Sectors,        // points to an in-memory copy of the 11 sectors
    USHORT   BytesPerSector
);