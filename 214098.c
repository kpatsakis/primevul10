static int run_filter(struct archive_read* a, struct filter_info* flt) {
	int ret;
	struct rar5* rar = get_context(a);

	free(rar->cstate.filtered_buf);

	rar->cstate.filtered_buf = malloc(flt->block_length);
	if(!rar->cstate.filtered_buf) {
		archive_set_error(&a->archive, ENOMEM,
		    "Can't allocate memory for filter data.");
		return ARCHIVE_FATAL;
	}

	switch(flt->type) {
		case FILTER_DELTA:
			ret = run_delta_filter(rar, flt);
			break;

		case FILTER_E8:
			/* fallthrough */
		case FILTER_E8E9:
			ret = run_e8e9_filter(rar, flt,
			    flt->type == FILTER_E8E9);
			break;

		case FILTER_ARM:
			ret = run_arm_filter(rar, flt);
			break;

		default:
			archive_set_error(&a->archive,
			    ARCHIVE_ERRNO_FILE_FORMAT,
			    "Unsupported filter type: 0x%x", flt->type);
			return ARCHIVE_FATAL;
	}

	if(ret != ARCHIVE_OK) {
		/* Filter has failed. */
		return ret;
	}

	if(ARCHIVE_OK != push_data_ready(a, rar, rar->cstate.filtered_buf,
	    flt->block_length, rar->cstate.last_write_ptr))
	{
		archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
		    "Stack overflow when submitting unpacked data");

		return ARCHIVE_FATAL;
	}

	rar->cstate.last_write_ptr += flt->block_length;
	return ARCHIVE_OK;
}