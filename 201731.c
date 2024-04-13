struct sigqueue *sigqueue_alloc(void)
{
	struct sigqueue *q;

	if ((q = __sigqueue_alloc(current, GFP_KERNEL, 0)))
		q->flags |= SIGQUEUE_PREALLOC;
	return(q);
}