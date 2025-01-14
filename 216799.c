newSWFInput_bufferCopy(unsigned char *buffer, int length)
{
	SWFInput input;
	unsigned char *data = (unsigned char *)malloc(length);
	if(data == NULL)
		return NULL;

	memcpy(data, buffer, length);
	input = newSWFInput_buffer(data, length);
	input->destroy = SWFInput_buffer_dtor;
#if TRACK_ALLOCS
	input->gcnode = ming_gc_add_node(input, (dtorfunctype) destroySWFInput);
#endif
	return input;
}