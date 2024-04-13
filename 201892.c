	__releases(rose_list_lock)
{
	spin_unlock_bh(&rose_list_lock);
}