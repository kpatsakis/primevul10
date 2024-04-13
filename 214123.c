static void cdeque_free(struct cdeque* d) {
	if(!d)
		return;

	if(!d->arr)
		return;

	free(d->arr);

	d->arr = NULL;
	d->beg_pos = -1;
	d->end_pos = -1;
	d->cap_mask = 0;
}