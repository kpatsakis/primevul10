static int read_bits_32(struct rar5* rar, const uint8_t* p, uint32_t* value) {
	uint32_t bits = ((uint32_t) p[rar->bits.in_addr]) << 24;
	bits |= p[rar->bits.in_addr + 1] << 16;
	bits |= p[rar->bits.in_addr + 2] << 8;
	bits |= p[rar->bits.in_addr + 3];
	bits <<= rar->bits.bit_addr;
	bits |= p[rar->bits.in_addr + 4] >> (8 - rar->bits.bit_addr);
	*value = bits;
	return ARCHIVE_OK;
}