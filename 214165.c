static int process_main_locator_extra_block(struct archive_read* a,
    struct rar5* rar)
{
	uint64_t locator_flags;

	enum LOCATOR_FLAGS {
		QLIST = 0x01, RECOVERY = 0x02,
	};

	if(!read_var(a, &locator_flags, NULL)) {
		return ARCHIVE_EOF;
	}

	if(locator_flags & QLIST) {
		if(!read_var(a, &rar->qlist_offset, NULL)) {
			return ARCHIVE_EOF;
		}

		/* qlist is not used */
	}

	if(locator_flags & RECOVERY) {
		if(!read_var(a, &rar->rr_offset, NULL)) {
			return ARCHIVE_EOF;
		}

		/* rr is not used */
	}

	return ARCHIVE_OK;
}