static struct list_head *virtual_queue(struct virtual_engine *ve)
{
	return &ve->base.execlists.default_priolist.requests[0];
}