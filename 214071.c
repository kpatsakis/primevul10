static int rar5_cleanup(struct archive_read *a) {
	struct rar5* rar = get_context(a);

	free(rar->cstate.window_buf);
	free(rar->cstate.filtered_buf);

	free(rar->vol.push_buf);

	free_filters(rar);
	cdeque_free(&rar->cstate.filters);

	free(rar);
	a->format->data = NULL;

	return ARCHIVE_OK;
}