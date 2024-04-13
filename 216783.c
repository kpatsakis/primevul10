SWFInput_stream_seek(SWFInput input, long offset, int whence)
{
	int len, l, readOffset;
	struct SWFInputStreamData *data;

	if ( whence == SEEK_CUR )
		input->offset = input->offset + offset;

	else if ( whence == SEEK_SET )
		input->offset = offset;

	else if ( whence == SEEK_END )
	{
		/* suck data until EOF */
		/* XXX - might want to put a limit on how much we suck (ha) */

		while ( SWFInput_getChar(input) != EOF )
			if (input->length > MAX_INPUTSTREAM)
				break;

		input->offset = input->length - offset;
	}

	if (input->offset < 0)
		input->offset = 0;

	if ( input->offset < input->length )
		return;

	/* now slurp up as much data as we need to get here */

	len = (((input->offset - input->length)/INPUTSTREAM_INCREMENT)+1) * INPUTSTREAM_INCREMENT;

	readOffset = input->length;
	input->length += len;
	data = (struct SWFInputStreamData*) input->data;

	data->buffer = (unsigned char*)realloc(data->buffer, sizeof(unsigned char) * (input->length + len));

	l=1; /* just to initialize to something */
	while((len > 0) && (l > 0))
	{
		l = fread(data->buffer, sizeof(unsigned char), len, data->file);
		len -= l;
		readOffset += l;
	}
}