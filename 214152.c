static void* cdeque_filter(struct filter_info* f) {
	return (void**) (size_t) f;
}