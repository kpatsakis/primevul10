ext4_next_entry(struct ext4_dir_entry_2 *p)
{
	return (struct ext4_dir_entry_2 *)((char *)p +
		ext4_rec_len_from_disk(p->rec_len));
}