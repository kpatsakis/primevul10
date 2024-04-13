static inline bool io_req_needs_clean(struct io_kiocb *req)
{
	return req->flags & IO_REQ_CLEAN_FLAGS;
}