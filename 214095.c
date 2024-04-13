static int cdeque_pop_front(struct cdeque* d, void** value) {
	if(!d || !value)
		return CDE_PARAM;

	if(d->size == 0)
		return CDE_OUT_OF_BOUNDS;

	cdeque_pop_front_fast(d, value);
	return CDE_OK;
}