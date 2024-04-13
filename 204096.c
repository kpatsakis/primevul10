static unsigned long fli_read_long(FILE *f)
{
	unsigned char b[4];
	fread(&b,1,4,f);
	return (unsigned long)(b[3]<<24) | (b[2]<<16) | (b[1]<<8) | b[0];
}