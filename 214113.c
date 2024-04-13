static int do_unstore_file(struct archive_read* a,
    struct rar5* rar, const void** buf, size_t* size, int64_t* offset)
{
	size_t to_read;
	const uint8_t* p;

	if(rar->file.bytes_remaining == 0 && rar->main.volume > 0 &&
	    rar->generic.split_after > 0)
	{
		int ret;

		rar->cstate.switch_multivolume = 1;
		ret = advance_multivolume(a);
		rar->cstate.switch_multivolume = 0;

		if(ret != ARCHIVE_OK) {
			/* Failed to advance to next multivolume archive
			 * file. */
			return ret;
		}
	}

	to_read = rar5_min(rar->file.bytes_remaining, 64 * 1024);
	if(to_read == 0) {
		return ARCHIVE_EOF;
	}

	if(!read_ahead(a, to_read, &p)) {
		archive_set_error(&a->archive, ARCHIVE_ERRNO_FILE_FORMAT,
		    "I/O error when unstoring file");
		return ARCHIVE_FATAL;
	}

	if(ARCHIVE_OK != consume(a, to_read)) {
		return ARCHIVE_EOF;
	}

	if(buf)    *buf = p;
	if(size)   *size = to_read;
	if(offset) *offset = rar->cstate.last_unstore_ptr;

	rar->file.bytes_remaining -= to_read;
	rar->cstate.last_unstore_ptr += to_read;

	update_crc(rar, p, to_read);
	return ARCHIVE_OK;
}