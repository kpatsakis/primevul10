static int apply_filters(struct archive_read* a) {
	struct filter_info* flt;
	struct rar5* rar = get_context(a);
	int ret;

	rar->cstate.all_filters_applied = 0;

	/* Get the first filter that can be applied to our data. The data
	 * needs to be fully unpacked before the filter can be run. */
	if(CDE_OK == cdeque_front(&rar->cstate.filters,
	    cdeque_filter_p(&flt))) {
		/* Check if our unpacked data fully covers this filter's
		 * range. */
		if(rar->cstate.write_ptr > flt->block_start &&
		    rar->cstate.write_ptr >= flt->block_start +
		    flt->block_length) {
			/* Check if we have some data pending to be written
			 * right before the filter's start offset. */
			if(rar->cstate.last_write_ptr == flt->block_start) {
				/* Run the filter specified by descriptor
				 * `flt`. */
				ret = run_filter(a, flt);
				if(ret != ARCHIVE_OK) {
					/* Filter failure, return error. */
					return ret;
				}

				/* Filter descriptor won't be needed anymore
				 * after it's used, * so remove it from the
				 * filter list and free its memory. */
				(void) cdeque_pop_front(&rar->cstate.filters,
				    cdeque_filter_p(&flt));

				free(flt);
			} else {
				/* We can't run filters yet, dump the memory
				 * right before the filter. */
				push_window_data(a, rar,
				    rar->cstate.last_write_ptr,
				    flt->block_start);
			}

			/* Return 'filter applied or not needed' state to the
			 * caller. */
			return ARCHIVE_RETRY;
		}
	}

	rar->cstate.all_filters_applied = 1;
	return ARCHIVE_OK;
}