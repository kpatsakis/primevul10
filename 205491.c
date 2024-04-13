JPEGPrintDir(TIFF* tif, FILE* fd, long flags)
{
	JPEGState* sp = JState(tif);

	assert(sp != NULL);
	(void) flags;

        if( sp != NULL ) {
		if (TIFFFieldSet(tif,FIELD_JPEGTABLES))
			fprintf(fd, "  JPEG Tables: (%lu bytes)\n",
				(unsigned long) sp->jpegtables_length);
		if (sp->printdir)
			(*sp->printdir)(tif, fd, flags);
	}
}