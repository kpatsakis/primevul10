int __page_symlink(struct inode *inode, const char *symname, int len,
		gfp_t gfp_mask)
{
	struct address_space *mapping = inode->i_mapping;
	struct page *page;
	void *fsdata;
	int err;
	char *kaddr;

retry:
	err = pagecache_write_begin(NULL, mapping, 0, len-1,
				AOP_FLAG_UNINTERRUPTIBLE, &page, &fsdata);
	if (err)
		goto fail;

	kaddr = kmap_atomic(page, KM_USER0);
	memcpy(kaddr, symname, len-1);
	kunmap_atomic(kaddr, KM_USER0);

	err = pagecache_write_end(NULL, mapping, 0, len-1, len-1,
							page, fsdata);
	if (err < 0)
		goto fail;
	if (err < len-1)
		goto retry;

	mark_inode_dirty(inode);
	return 0;
fail:
	return err;
}