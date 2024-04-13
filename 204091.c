static void fli_write_short(FILE *f, unsigned short w)
{
	unsigned char b[2];
	b[0]=w&255;
	b[1]=(w>>8)&255;
	fwrite(&b,1,2,f);
}