static void init_unpack(struct rar5* rar) {
	rar->file.calculated_crc32 = 0;
	init_window_mask(rar);

	free(rar->cstate.window_buf);
	free(rar->cstate.filtered_buf);

	if(rar->cstate.window_size > 0) {
		rar->cstate.window_buf = calloc(1, rar->cstate.window_size);
		rar->cstate.filtered_buf = calloc(1, rar->cstate.window_size);
	} else {
		rar->cstate.window_buf = NULL;
		rar->cstate.filtered_buf = NULL;
	}

	rar->cstate.write_ptr = 0;
	rar->cstate.last_write_ptr = 0;

	memset(&rar->cstate.bd, 0, sizeof(rar->cstate.bd));
	memset(&rar->cstate.ld, 0, sizeof(rar->cstate.ld));
	memset(&rar->cstate.dd, 0, sizeof(rar->cstate.dd));
	memset(&rar->cstate.ldd, 0, sizeof(rar->cstate.ldd));
	memset(&rar->cstate.rd, 0, sizeof(rar->cstate.rd));
}