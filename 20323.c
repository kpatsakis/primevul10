
static bool io_alloc_file_tables(struct io_file_table *table, unsigned nr_files)
{
	size_t size = nr_files * sizeof(struct io_fixed_file);

	table->files = (struct io_fixed_file **)io_alloc_page_table(size);
	return !!table->files;