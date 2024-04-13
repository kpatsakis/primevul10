static int read_consume_bits(struct rar5* rar, const uint8_t* p, int n,
    int* value)
{
	uint16_t v;
	int ret, num;

	if(n == 0 || n > 16) {
		/* This is a programmer error and should never happen
		 * in runtime. */
		return ARCHIVE_FATAL;
	}

	ret = read_bits_16(rar, p, &v);
	if(ret != ARCHIVE_OK)
		return ret;

	num = (int) v;
	num >>= 16 - n;

	skip_bits(rar, n);

	if(value)
		*value = num;

	return ARCHIVE_OK;
}