static int read_ahead(struct archive_read* a, size_t how_many,
    const uint8_t** ptr)
{
	ssize_t avail = -1;
	if(!ptr)
		return 0;

	*ptr = __archive_read_ahead(a, how_many, &avail);
	if(*ptr == NULL) {
		return 0;
	}

	return 1;
}