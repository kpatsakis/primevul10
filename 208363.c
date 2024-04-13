static unsigned char * xor(unsigned char *p, unsigned char *q, int length)
{
	int i;
	unsigned char *retval = p;

	for (i = 0; i < length; i++) {
		*(p++) ^= *(q++);
	}
	return retval;
}