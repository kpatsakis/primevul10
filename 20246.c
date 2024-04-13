
static void io_free_file_tables(struct io_file_table *table, unsigned nr_files)
{
	size_t size = nr_files * sizeof(struct io_fixed_file);

	io_free_page_table((void **)table->files, size);
	table->files = NULL;