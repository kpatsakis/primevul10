static int read_u64(struct archive_read* a, uint64_t* pvalue) {
	const uint8_t* p;
	if(!read_ahead(a, 8, &p))
		return 0;

	*pvalue = archive_le64dec(p);
	return ARCHIVE_OK == consume(a, 8) ? 1 : 0;
}