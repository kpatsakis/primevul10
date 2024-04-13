SWFInput_stream_getChar(SWFInput input)
{
	struct SWFInputStreamData *data = (struct SWFInputStreamData *) input->data;

	if (input->offset >= MAX_INPUTSTREAM)
		return EOF;

	if ( input->offset == input->length )
	{
		/* fetch from stream, save in buffer */

		FILE *f = data->file;
		int c = fgetc(f);

		++input->offset;

		if ( c != EOF )
		{
			if ( input->length % INPUTSTREAM_INCREMENT == 0 )
			{
				data->buffer = (unsigned char*) realloc(data->buffer,
															 sizeof(unsigned char) *
															 (input->length + INPUTSTREAM_INCREMENT));
			}

			data->buffer[input->length] = c;
			++input->length;
		}

		return c;
	}
	else if ( input->offset < input->length )
	{
		/* fetch from buffer */
		return data->buffer[input->offset++];
	}
	else
		return EOF;
}