lock_hrtimer_base(const struct hrtimer *timer, unsigned long *flags)
{
	struct hrtimer_clock_base *base = timer->base;

	spin_lock_irqsave(&base->cpu_base->lock, *flags);

	return base;
}