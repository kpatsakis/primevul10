shmem_write_end(struct file *file, struct address_space *mapping,
			loff_t pos, unsigned len, unsigned copied,
			struct page *page, void *fsdata)
{
	struct inode *inode = mapping->host;

	set_page_dirty(page);
	page_cache_release(page);

	if (pos+copied > inode->i_size)
		i_size_write(inode, pos+copied);

	return copied;
}