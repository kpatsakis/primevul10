static void cdeque_clear(struct cdeque* d) {
	d->size = 0;
	d->beg_pos = 0;
	d->end_pos = 0;
}