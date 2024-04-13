newSWFInput_stream(FILE* f)
{
	SWFInput input = (SWFInput)malloc(sizeof(struct SWFInput_s));
	struct SWFInputStreamData *data;

	/* If malloc failed, return NULL to signify this */
	if (NULL == input)
		return NULL;

	data = (struct SWFInputStreamData *)malloc(sizeof(struct SWFInputStreamData));

	/* If malloc failed, free memory allocated for input and return NULL to signify the failure */
	if (NULL == data)
	{
		free(input);
		return NULL;
	}

	input->getChar = SWFInput_stream_getChar;
	input->destroy = SWFInput_stream_dtor;
	input->eof = SWFInput_stream_eof;
	input->read = SWFInput_stream_read;
	input->seek = SWFInput_stream_seek;

	input->offset = 0;
	input->length = 0;
	input->buffer = 0;
	input->bufbits = 0;

	data->file = f;
	data->buffer = NULL;

	input->data = (void *)data;

#if TRACK_ALLOCS
	input->gcnode = ming_gc_add_node(input, (dtorfunctype) destroySWFInput);
#endif

	return input;
}