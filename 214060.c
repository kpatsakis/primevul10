static int run_delta_filter(struct rar5* rar, struct filter_info* flt) {
	int i;
	ssize_t dest_pos, src_pos = 0;

	for(i = 0; i < flt->channels; i++) {
		uint8_t prev_byte = 0;
		for(dest_pos = i;
				dest_pos < flt->block_length;
				dest_pos += flt->channels)
		{
			uint8_t byte;

			byte = rar->cstate.window_buf[
			    (rar->cstate.solid_offset + flt->block_start +
			    src_pos) & rar->cstate.window_mask];

			prev_byte -= byte;
			rar->cstate.filtered_buf[dest_pos] = prev_byte;
			src_pos++;
		}
	}

	return ARCHIVE_OK;
}