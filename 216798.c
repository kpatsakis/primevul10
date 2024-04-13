newSWFInput_input(SWFInput in, unsigned int length)
{
	SWFInput input;
	struct SWFInputPtr *data;

	if (in == NULL)
		return NULL;

	input = (SWFInput)malloc(sizeof(struct SWFInput_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == input)
		return NULL;

	input->getChar = SWFInput_input_getChar;
	input->destroy = SWFInput_input_dtor;
	input->eof = SWFInput_input_eof;
	input->read = SWFInput_input_read;
	input->seek = SWFInput_input_seek;

	data = (struct SWFInputPtr *)malloc(sizeof(struct SWFInputPtr));

	/* If malloc failed, free memory allocated for input and return NULL to signify the failure */
	if (NULL == data)
	{
		free(input);
		return NULL;
	}

	data->offset = SWFInput_tell(in);
	data->input = in;

	input->offset = 0;
	input->length = length;

	input->data = (void *)data;
	input->buffer = 0;
	input->bufbits = 0;
#if TRACK_ALLOCS
	input->gcnode = ming_gc_add_node(input, (dtorfunctype) destroySWFInput);
#endif

	return input;
}