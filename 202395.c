static int __init tc_action_init(void)
{
	struct rtnetlink_link *link_p = rtnetlink_links[PF_UNSPEC];

	if (link_p) {
		link_p[RTM_NEWACTION-RTM_BASE].doit = tc_ctl_action;
		link_p[RTM_DELACTION-RTM_BASE].doit = tc_ctl_action;
		link_p[RTM_GETACTION-RTM_BASE].doit = tc_ctl_action;
		link_p[RTM_GETACTION-RTM_BASE].dumpit = tc_dump_action;
	}

	printk("TC classifier action (bugs to netdev@vger.kernel.org cc "
	       "hadi@cyberus.ca)\n");
	return 0;
}