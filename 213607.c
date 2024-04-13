nfs4_ff_start_busy_timer(struct nfs4_ff_busy_timer *timer, ktime_t now)
{
	/* first IO request? */
	if (atomic_inc_return(&timer->n_ops) == 1) {
		timer->start_time = now;
	}
}