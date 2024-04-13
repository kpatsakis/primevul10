static int use_data(struct rar5* rar, const void** buf, size_t* size,
    int64_t* offset)
{
	int i;

	for(i = 0; i < rar5_countof(rar->cstate.dready); i++) {
		struct data_ready *d = &rar->cstate.dready[i];

		if(d->used) {
			if(buf)    *buf = d->buf;
			if(size)   *size = d->size;
			if(offset) *offset = d->offset;

			d->used = 0;
			return ARCHIVE_OK;
		}
	}

	return ARCHIVE_RETRY;
}