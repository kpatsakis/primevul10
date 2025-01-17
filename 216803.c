newSWFInput_buffer(unsigned char* buffer, int length)
{
	SWFInput input = (SWFInput) malloc(sizeof(struct SWFInput_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == input)
		return NULL;

	input->getChar = SWFInput_buffer_getChar;
	input->destroy = SWFInput_dtor;
	input->eof = SWFInput_buffer_eof;
	input->read = SWFInput_buffer_read;
	input->seek = SWFInput_buffer_seek;
	input->data = buffer;
	input->buffer = 0;
	input->bufbits = 0;
	input->offset = 0;
	input->length = length;

#if TRACK_ALLOCS
	input->gcnode = ming_gc_add_node(input, (dtorfunctype) destroySWFInput);
#endif

	return input;
}