xmallox (size_t len)
{
	void *memres = malloc (len);
	if (memres == NULL) {
		fprintf(stderr, "Running Out of Memory!!!\n");
		exit(EXIT_FAILURE);
	}
	return memres;
}