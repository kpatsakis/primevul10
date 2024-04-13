print_hline (unsigned int ids, unsigned int screenwidth)
{
        unsigned int i, width;

        width = ids ? 6 : 20; 
        for (i = 0; i < width; i++)
		fputc ('-', stdout);
        fputc ('+', stdout);
        for (i = 0; i < screenwidth - 2 - width; i++)
		fputc ('-', stdout);
	fputc ('\n', stdout);
}