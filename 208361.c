static attribute_t *find_attribute(AUTH_HDR *response, unsigned char type)
{
	attribute_t *attr = (attribute_t *) &response->data;

	int len = ntohs(response->length) - AUTH_HDR_LEN;

	while (attr->attribute != type) {
		if ((len -= attr->length) <= 0) {
			return NULL;		/* not found */
		}
		attr = (attribute_t *) ((char *) attr + attr->length);
	}

	return attr;
}