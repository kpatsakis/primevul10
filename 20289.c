static void io_free_req_deferred(struct io_kiocb *req)
{
	req->io_task_work.func = io_free_req;
	io_req_task_work_add(req);
}