SWFInput_buffer_dtor(SWFInput input)
{
	free(input->data);
#if TRACK_ALLOCS
	ming_gc_remove_node(input->gcnode);
#endif
	free(input);
}