static int push_data_ready(struct archive_read* a, struct rar5* rar,
    const uint8_t* buf, size_t size, int64_t offset)
{
	int i;

	/* Don't push if we're in skip mode. This is needed because solid
	 * streams need full processing even if we're skipping data. After
	 * fully processing the stream, we need to discard the generated bytes,
	 * because we're interested only in the side effect: building up the
	 * internal window circular buffer. This window buffer will be used
	 * later during unpacking of requested data. */
	if(rar->skip_mode)
		return ARCHIVE_OK;

	/* Sanity check. */
	if(offset != rar->file.last_offset + rar->file.last_size) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
		    "Sanity check error: output stream is not continuous");
		return ARCHIVE_FATAL;
	}

	for(i = 0; i < rar5_countof(rar->cstate.dready); i++) {
		struct data_ready* d = &rar->cstate.dready[i];
		if(!d->used) {
			d->used = 1;
			d->buf = buf;
			d->size = size;
			d->offset = offset;

			/* These fields are used only in sanity checking. */
			rar->file.last_offset = offset;
			rar->file.last_size = size;

			/* Calculate the checksum of this new block before
			 * submitting data to libarchive's engine. */
			update_crc(rar, d->buf, d->size);

			return ARCHIVE_OK;
		}
	}

	/* Program counter will reach this code if the `rar->cstate.data_ready`
	 * stack will be filled up so that no new entries will be allowed. The
	 * code shouldn't allow such situation to occur. So we treat this case
	 * as an internal error. */

	archive_set_error(&a->archive, ARCHIVE_ERRNO_PROGRAMMER,
	    "Error: premature end of data_ready stack");
	return ARCHIVE_FATAL;
}