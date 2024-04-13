processCompressOptions(char* opt)
{
	if (streq(opt, "none")) {
		defcompression = COMPRESSION_NONE;
	} else if (streq(opt, "packbits")) {
		defcompression = COMPRESSION_PACKBITS;
	} else if (strneq(opt, "jpeg", 4)) {
		char* cp = strchr(opt, ':');

		defcompression = COMPRESSION_JPEG;
		while( cp )
		{
			if (isdigit((int)cp[1]))
				quality = atoi(cp+1);
			else if (cp[1] == 'r' )
				jpegcolormode = JPEGCOLORMODE_RAW;
			else
				usage();

			cp = strchr(cp+1,':');
		}
	} else if (strneq(opt, "g3", 2)) {
		processG3Options(opt);
		defcompression = COMPRESSION_CCITTFAX3;
	} else if (streq(opt, "g4")) {
		defcompression = COMPRESSION_CCITTFAX4;
	} else if (strneq(opt, "lzw", 3)) {
		char* cp = strchr(opt, ':');
		if (cp)
			defpredictor = atoi(cp+1);
		defcompression = COMPRESSION_LZW;
	} else if (strneq(opt, "zip", 3)) {
		processZIPOptions(opt);
		defcompression = COMPRESSION_ADOBE_DEFLATE;
	} else if (strneq(opt, "lzma", 4)) {
		processZIPOptions(opt);
		defcompression = COMPRESSION_LZMA;
	} else if (strneq(opt, "jbig", 4)) {
		defcompression = COMPRESSION_JBIG;
	} else if (strneq(opt, "sgilog", 6)) {
		defcompression = COMPRESSION_SGILOG;
	} else
		return (0);
	return (1);
}