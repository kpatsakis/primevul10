void __init rtnetlink_init(void)
{
	int i;

	rtattr_max = 0;
	for (i = 0; i < ARRAY_SIZE(rta_max); i++)
		if (rta_max[i] > rtattr_max)
			rtattr_max = rta_max[i];
	rta_buf = kmalloc(rtattr_max * sizeof(struct rtattr *), GFP_KERNEL);
	if (!rta_buf)
		panic("rtnetlink_init: cannot allocate rta_buf\n");

	rtnl = netlink_kernel_create(NETLINK_ROUTE, rtnetlink_rcv);
	if (rtnl == NULL)
		panic("rtnetlink_init: cannot initialize rtnetlink\n");
	netlink_set_nonroot(NETLINK_ROUTE, NL_NONROOT_RECV);
	register_netdevice_notifier(&rtnetlink_dev_notifier);
	rtnetlink_links[PF_UNSPEC] = link_rtnetlink_table;
	rtnetlink_links[PF_PACKET] = link_rtnetlink_table;
}