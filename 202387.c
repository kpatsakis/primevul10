static int __init pktsched_init(void)
{
	struct rtnetlink_link *link_p;

#ifdef CONFIG_NET_SCH_CLK_CPU
	if (psched_calibrate_clock() < 0)
		return -1;
#elif defined(CONFIG_NET_SCH_CLK_JIFFIES)
	psched_tick_per_us = HZ<<PSCHED_JSCALE;
	psched_us_per_tick = 1000000;
#endif

	link_p = rtnetlink_links[PF_UNSPEC];

	/* Setup rtnetlink links. It is made here to avoid
	   exporting large number of public symbols.
	 */

	if (link_p) {
		link_p[RTM_NEWQDISC-RTM_BASE].doit = tc_modify_qdisc;
		link_p[RTM_DELQDISC-RTM_BASE].doit = tc_get_qdisc;
		link_p[RTM_GETQDISC-RTM_BASE].doit = tc_get_qdisc;
		link_p[RTM_GETQDISC-RTM_BASE].dumpit = tc_dump_qdisc;
		link_p[RTM_NEWTCLASS-RTM_BASE].doit = tc_ctl_tclass;
		link_p[RTM_DELTCLASS-RTM_BASE].doit = tc_ctl_tclass;
		link_p[RTM_GETTCLASS-RTM_BASE].doit = tc_ctl_tclass;
		link_p[RTM_GETTCLASS-RTM_BASE].dumpit = tc_dump_tclass;
	}

	register_qdisc(&pfifo_qdisc_ops);
	register_qdisc(&bfifo_qdisc_ops);
	proc_net_fops_create("psched", 0, &psched_fops);

	return 0;
}