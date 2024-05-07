#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <zlib.h>

#include "fast_fwd_crc.h"


static uint32_t zlib_crc(uint32_t state, const char *data, size_t len)
{
	while (len > INT_MAX) {
		state = crc32(state, (const Bytef*)data, INT_MAX);
		data += INT_MAX;
		len -= INT_MAX;
	}
	return crc32(state, (const Bytef*)data, len);
}

int main(int argc, char *argv[])
{
	if (argc != 4) {
		if (argc != 0) {
			fprintf(stderr, "usage: %s <prefix-data> <hex-padding-byte> <padding-length>\n", argv[0]);
		}
		return 1;
	}


	char *prefix_data = argv[1];
	size_t prefix_len = strlen(prefix_data);
	char *outp;
	unsigned long pad_byte = strtoul(argv[2], &outp, 16);
	if (!argv[2][0] || outp[0] || pad_byte > 0xff) {
		fprintf(stderr, "error: invalid padding byte\n");
		return 1;
	}
	unsigned long pad_len = strtoul(argv[3], &outp, 0);
	if (!argv[2][0] || outp[0] || pad_len > SIZE_MAX - prefix_len) {
		fprintf(stderr, "error: invalid padding length\n");
		return 1;
	}


	size_t buf_len = prefix_len + pad_len;

	char *buf = malloc(buf_len);
	if (buf == NULL)
		abort();
	memcpy(buf, prefix_data, prefix_len);
	memset(buf + prefix_len, pad_byte, pad_len);

	uint32_t prefix_crc = zlib_crc(0, buf, prefix_len);
	printf("prefix CRC:      %08"PRIx32"\n", prefix_crc);

	uint32_t full_crc_zlib = zlib_crc(0, buf, buf_len);
	printf("full CRC [zlib]: %08"PRIx32"\n", full_crc_zlib);

	uint32_t full_crc_ff = fast_fwd_crc32(prefix_crc, pad_byte, pad_len);
	printf("full CRC [ff]:   %08" PRIx32 "\n", full_crc_ff);
	return full_crc_zlib != full_crc_ff;
}
