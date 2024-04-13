static int cdeque_push_back(struct cdeque* d, void* item) {
	if(d == NULL)
		return CDE_PARAM;

	if(d->size == d->cap_mask + 1)
		return CDE_OUT_OF_BOUNDS;

	d->arr[d->end_pos] = (size_t) item;
	d->end_pos = (d->end_pos + 1) & d->cap_mask;
	d->size++;

	return CDE_OK;
}