static inline struct rar5* get_context(struct archive_read* a) {
	return (struct rar5*) a->format->data;
}