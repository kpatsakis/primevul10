static int __ext4_normal_writepage(struct page *page,
				struct writeback_control *wbc)
{
	struct inode *inode = page->mapping->host;

	if (test_opt(inode->i_sb, NOBH))
		return nobh_writepage(page,
					ext4_normal_get_block_write, wbc);
	else
		return block_write_full_page(page,
						ext4_normal_get_block_write,
						wbc);
}