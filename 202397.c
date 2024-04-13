static int wireless_seq_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &wireless_seq_ops);
}