SWFInput_stream_read(SWFInput input, unsigned char* buffer, int count)
{
	struct SWFInputStreamData *data = (struct SWFInputStreamData *) input->data;
	int need = input->offset + count - input->length;

	if ( need > 0 )
	{
		int num;

		data->buffer =
			(unsigned char*) realloc(data->buffer,
							sizeof(unsigned char) * INPUTSTREAM_INCREMENT *
							(((input->offset + count) / INPUTSTREAM_INCREMENT) + 1));
		
		num = fread(data->buffer + input->length,
								sizeof(unsigned char), need, data->file);

		input->length += num;
	}

	if ( count > input->length - input->offset )
		count = input->length - input->offset;

	memcpy(buffer, data->buffer + input->offset, count);

	return count;
}