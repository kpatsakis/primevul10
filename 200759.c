static int pipe_to_sendpage(struct pipe_inode_info *pipe,
			    struct pipe_buffer *buf, struct splice_desc *sd)
{
	struct file *file = sd->file;
	loff_t pos = sd->pos;
	int ret, more;

	ret = buf->ops->pin(pipe, buf);
	if (!ret) {
		more = (sd->flags & SPLICE_F_MORE) || sd->len < sd->total_len;

		ret = file->f_op->sendpage(file, buf->page, buf->offset,
					   sd->len, &pos, more);
	}

	return ret;
}