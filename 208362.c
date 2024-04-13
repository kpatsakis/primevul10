static void add_attribute(AUTH_HDR *request, unsigned char type, CONST unsigned char *data, int length)
{
	attribute_t *p;

	p = (attribute_t *) ((unsigned char *)request + ntohs(request->length));
	p->attribute = type;
	p->length = length + 2;		/* the total size of the attribute */
	request->length = htons(ntohs(request->length) + p->length);
	memcpy(p->data, data, length);
}