int ext4_page_mkwrite(struct vm_area_struct *vma, struct page *page)
{
	loff_t size;
	unsigned long len;
	int ret = -EINVAL;
	void *fsdata;
	struct file *file = vma->vm_file;
	struct inode *inode = file->f_path.dentry->d_inode;
	struct address_space *mapping = inode->i_mapping;

	/*
	 * Get i_alloc_sem to stop truncates messing with the inode. We cannot
	 * get i_mutex because we are already holding mmap_sem.
	 */
	down_read(&inode->i_alloc_sem);
	size = i_size_read(inode);
	if (page->mapping != mapping || size <= page_offset(page)
	    || !PageUptodate(page)) {
		/* page got truncated from under us? */
		goto out_unlock;
	}
	ret = 0;
	if (PageMappedToDisk(page))
		goto out_unlock;

	if (page->index == size >> PAGE_CACHE_SHIFT)
		len = size & ~PAGE_CACHE_MASK;
	else
		len = PAGE_CACHE_SIZE;

	if (page_has_buffers(page)) {
		/* return if we have all the buffers mapped */
		if (!walk_page_buffers(NULL, page_buffers(page), 0, len, NULL,
				       ext4_bh_unmapped))
			goto out_unlock;
	}
	/*
	 * OK, we need to fill the hole... Do write_begin write_end
	 * to do block allocation/reservation.We are not holding
	 * inode.i__mutex here. That allow * parallel write_begin,
	 * write_end call. lock_page prevent this from happening
	 * on the same page though
	 */
	ret = mapping->a_ops->write_begin(file, mapping, page_offset(page),
			len, AOP_FLAG_UNINTERRUPTIBLE, &page, &fsdata);
	if (ret < 0)
		goto out_unlock;
	ret = mapping->a_ops->write_end(file, mapping, page_offset(page),
			len, len, page, fsdata);
	if (ret < 0)
		goto out_unlock;
	ret = 0;
out_unlock:
	up_read(&inode->i_alloc_sem);
	return ret;
}