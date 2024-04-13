static unsigned char fli_read_char(FILE *f)
{
	unsigned char b;
	fread(&b,1,1,f);
	return b;
}