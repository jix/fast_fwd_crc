#ifndef FAST_FWD_CRC_H
#define FAST_FWD_CRC_H

#include <stdint.h>

// Given a CRC-32 `state` for some prefix P, compute the CRC-32 of P with `len`
// copies of the byte `padding` appended.
uint32_t fast_fwd_crc32(uint32_t state, unsigned char padding, uint32_t len);

#endif
