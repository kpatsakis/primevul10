static uint32_t read_filter_data(struct rar5* rar, uint32_t offset) {
	uint8_t linear_buf[4];
	circular_memcpy(linear_buf, rar->cstate.window_buf,
	    rar->cstate.window_mask, offset, offset + 4);
	return archive_le32dec(linear_buf);
}