HTTP_create(void *p, uint16_t nhttp, unsigned len)
{
	struct http *hp;

	hp = p;
	hp->magic = HTTP_MAGIC;
	hp->hd = (void*)(hp + 1);
	hp->shd = nhttp;
	hp->hdf = (void*)(hp->hd + nhttp);
	assert((unsigned char*)p + len == hp->hdf + PRNDUP(nhttp));
	return (hp);
}