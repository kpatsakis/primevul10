TIFFjpeg_error_exit(j_common_ptr cinfo)
{
	JPEGState *sp = (JPEGState *) cinfo;	/* NB: cinfo assumed first */
	char buffer[JMSG_LENGTH_MAX];

	(*cinfo->err->format_message) (cinfo, buffer);
	TIFFErrorExt(sp->tif->tif_clientdata, "JPEGLib", "%s", buffer);		/* display the error message */
	jpeg_abort(cinfo);			/* clean up libjpeg state */
	LONGJMP(sp->exit_jmpbuf, 1);		/* return to libtiff caller */
}