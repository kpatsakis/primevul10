static unsigned short fli_read_short(FILE *f)
{
	unsigned char b[2];
	fread(&b,1,2,f);
	return (unsigned short)(b[1]<<8) | b[0];
}