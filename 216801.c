newSWFInput_allocedBuffer(unsigned char *buffer, int length)
{
	SWFInput input = newSWFInput_buffer(buffer, length);
	input->destroy = SWFInput_buffer_dtor;
#if TRACK_ALLOCS
	input->gcnode = ming_gc_add_node(input, (dtorfunctype) destroySWFInput);
#endif
	return input;
}