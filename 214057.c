static void cdeque_pop_front_fast(struct cdeque* d, void** value) {
	*value = (void*) d->arr[d->beg_pos];
	d->beg_pos = (d->beg_pos + 1) & d->cap_mask;
	d->size--;
}