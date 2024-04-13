nfs4_ff_end_busy_timer(struct nfs4_ff_busy_timer *timer, ktime_t now)
{
	ktime_t start;

	if (atomic_dec_return(&timer->n_ops) < 0)
		WARN_ON_ONCE(1);

	start = timer->start_time;
	timer->start_time = now;
	return ktime_sub(now, start);
}