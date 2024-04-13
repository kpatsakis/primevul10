static int qdisc_change(struct Qdisc *sch, struct rtattr **tca)
{
	if (tca[TCA_OPTIONS-1]) {
		int err;

		if (sch->ops->change == NULL)
			return -EINVAL;
		err = sch->ops->change(sch, tca[TCA_OPTIONS-1]);
		if (err)
			return err;
	}
#ifdef CONFIG_NET_ESTIMATOR
	if (tca[TCA_RATE-1])
		gen_replace_estimator(&sch->bstats, &sch->rate_est,
			sch->stats_lock, tca[TCA_RATE-1]);
#endif
	return 0;
}