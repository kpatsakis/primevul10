ssize_t splice_from_pipe(struct pipe_inode_info *pipe, struct file *out,
			 loff_t *ppos, size_t len, unsigned int flags,
			 splice_actor *actor)
{
	ssize_t ret;
	struct inode *inode = out->f_mapping->host;

	/*
	 * The actor worker might be calling ->prepare_write and
	 * ->commit_write. Most of the time, these expect i_mutex to
	 * be held. Since this may result in an ABBA deadlock with
	 * pipe->inode, we have to order lock acquiry here.
	 */
	inode_double_lock(inode, pipe->inode);
	ret = __splice_from_pipe(pipe, out, ppos, len, flags, actor);
	inode_double_unlock(inode, pipe->inode);

	return ret;
}