static inline void req_set_fail(struct io_kiocb *req)
{
	req->flags |= REQ_F_FAIL;
}