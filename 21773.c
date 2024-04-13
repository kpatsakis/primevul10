HTTP_estimate(unsigned nhttp)
{

	/* XXX: We trust the structs to size-aligned as necessary */
	return (PRNDUP(sizeof(struct http) + sizeof(txt) * nhttp + nhttp));
}