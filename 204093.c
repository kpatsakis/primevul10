static void fli_write_long(FILE *f, unsigned long l)
{
	unsigned char b[4];
	b[0]=l&255;
	b[1]=(l>>8)&255;
	b[2]=(l>>16)&255;
	b[3]=(l>>24)&255;
	fwrite(&b,1,4,f);
}