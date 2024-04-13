static int read_u32(struct archive_read* a, uint32_t* pvalue) {
	const uint8_t* p;
	if(!read_ahead(a, 4, &p))
		return 0;

	*pvalue = archive_le32dec(p);
	return ARCHIVE_OK == consume(a, 4) ? 1 : 0;
}