int shmem_unuse(swp_entry_t entry, struct page *page)
{
	struct list_head *p, *next;
	struct shmem_inode_info *info;
	int found = 0;

	spin_lock(&shmem_swaplist_lock);
	list_for_each_safe(p, next, &shmem_swaplist) {
		info = list_entry(p, struct shmem_inode_info, swaplist);
		if (!info->swapped)
			list_del_init(&info->swaplist);
		else if (shmem_unuse_inode(info, entry, page)) {
			/* move head to start search for next from here */
			list_move_tail(&shmem_swaplist, &info->swaplist);
			found = 1;
			break;
		}
	}
	spin_unlock(&shmem_swaplist_lock);
	return found;
}