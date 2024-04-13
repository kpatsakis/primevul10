uint8_t bf_is_table_present(const struct compressed_block_header* hdr) {
	return (hdr->block_flags_u8 >> 7) & 1;
}