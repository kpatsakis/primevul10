static int cdeque_front(struct cdeque* d, void** value) {
	if(d->size > 0) {
		cdeque_front_fast(d, value);
		return CDE_OK;
	} else
		return CDE_OUT_OF_BOUNDS;
}