static void fli_write_char(FILE *f, unsigned char b)
{
	fwrite(&b,1,1,f);
}