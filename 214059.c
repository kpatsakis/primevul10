static void push_window_data(struct archive_read* a, struct rar5* rar,
    int64_t idx_begin, int64_t idx_end)
{
	push_data(a, rar, rar->cstate.window_buf, idx_begin, idx_end);
}