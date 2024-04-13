TIFFjpeg_output_message(j_common_ptr cinfo)
{
	char buffer[JMSG_LENGTH_MAX];

	(*cinfo->err->format_message) (cinfo, buffer);
	TIFFWarningExt(((JPEGState *) cinfo)->tif->tif_clientdata, "JPEGLib", "%s", buffer);
}