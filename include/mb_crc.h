
#ifndef __MB_CRC_H__
#define __MB_CRC_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint16_t
mb_crc16(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif  //!__MB_CRC_H__
