uint8_t bf_bit_size(const struct compressed_block_header* hdr) {
	return hdr->block_flags_u8 & 7;
}