static void addrconf_ip6_tnl_config(struct net_device *dev)
{
	struct inet6_dev *idev;

	ASSERT_RTNL();

	if ((idev = addrconf_add_dev(dev)) == NULL) {
		printk(KERN_DEBUG "init ip6-ip6: add_dev failed\n");
		return;
	}
	ip6_tnl_add_linklocal(idev);
	addrconf_add_mroute(dev);
}