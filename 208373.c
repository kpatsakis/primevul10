static void add_int_attribute(AUTH_HDR *request, unsigned char type, int data)
{
	int value = htonl(data);

	add_attribute(request, type, (unsigned char *) &value, sizeof(int));
}