static void cdeque_front_fast(struct cdeque* d, void** value) {
	*value = (void*) d->arr[d->beg_pos];
}