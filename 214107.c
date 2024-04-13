static struct filter_info* add_new_filter(struct rar5* rar) {
	struct filter_info* f =
		(struct filter_info*) calloc(1, sizeof(struct filter_info));

	if(!f) {
		return NULL;
	}

	cdeque_push_back(&rar->cstate.filters, cdeque_filter(f));
	return f;
}