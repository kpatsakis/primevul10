
void __io_uring_cancel(struct files_struct *files)
{
	io_uring_cancel_generic(!files, NULL);