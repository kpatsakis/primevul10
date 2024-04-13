static int io_accept(struct io_kiocb *req, unsigned int issue_flags)
{
	struct io_accept *accept = &req->accept;
	bool force_nonblock = issue_flags & IO_URING_F_NONBLOCK;
	unsigned int file_flags = force_nonblock ? O_NONBLOCK : 0;
	int ret;

	if (req->file->f_flags & O_NONBLOCK)
		req->flags |= REQ_F_NOWAIT;

	ret = __sys_accept4_file(req->file, file_flags, accept->addr,
					accept->addr_len, accept->flags,
					accept->nofile);
	if (ret == -EAGAIN && force_nonblock)
		return -EAGAIN;
	if (ret < 0) {
		if (ret == -ERESTARTSYS)
			ret = -EINTR;
		req_set_fail(req);
	}
	__io_req_complete(req, issue_flags, ret, 0);
	return 0;
}