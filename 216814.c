SWFInput_dtor(SWFInput input)
{
#if TRACK_ALLOCS
	ming_gc_remove_node(input->gcnode);
#endif
	free(input);
}