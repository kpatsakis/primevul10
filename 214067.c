uint8_t bf_byte_count(const struct compressed_block_header* hdr) {
	return (hdr->block_flags_u8 >> 3) & 7;
}