#include "fast_fwd_crc.h"

// Carryless 32x32 bit -> 64 bit multiplication
static uint64_t clmul32(uint32_t a, uint32_t b)
{
	// Some CPUs have a dedicated instruction for this. In any case this
	// can likely be optimized using small tables and/or bit-twiddling
	uint64_t out = 0;
	uint64_t b_shifted = b;
	while (a) {
		out ^= (a & 1) ? b_shifted : 0;
		b_shifted <<= 1;
		a >>= 1;
	}
	return out;
}

// Bitreversed 64 bit -> 32 bit modular reduction with the CRC polynomial
static uint32_t crc32_reduce(uint64_t wide)
{
	// This can also be optimized using small tables and/or bit-twiddling
	// or by using a fast clmul instruction
	for (int i = 0; i < 32; i++) {
		int lsb = wide & 1;
		wide = (wide >> 1) ^ (lsb ? 0xedb88320 : 0);
	}
	return (uint32_t)wide;
}

uint32_t fast_fwd_crc32(uint32_t state, unsigned char pad, size_t len)
{
	// initialize the affine function to appending a single pad byte
	uint32_t a = (uint32_t)1 << 24;
	uint32_t b = (uint32_t)pad << 24;

	// crc32 uses an inverted state, so we invert around updating it
	state = ~state;

	for (;;) {
		// pick out the powers of two that add to the given length
		if (len & 1) {
			// update the state by adding 2**i pad bytes
			state = crc32_reduce(clmul32(state, a) ^ b);
		}
		len >>= 1;
		if (!len) {
			break;
		}
		// double the number of appended pad bytes by self-compising
		// the affine function, giving us the next power of two
		b = crc32_reduce(clmul32(b, a)) ^ b;
		a = crc32_reduce(clmul32(a, a));
	}

	return ~state;
}
