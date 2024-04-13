static int read_bits_16(struct rar5* rar, const uint8_t* p, uint16_t* value) {
	int bits = (int) ((uint32_t) p[rar->bits.in_addr]) << 16;
	bits |= (int) p[rar->bits.in_addr + 1] << 8;
	bits |= (int) p[rar->bits.in_addr + 2];
	bits >>= (8 - rar->bits.bit_addr);
	*value = bits & 0xffff;
	return ARCHIVE_OK;
}