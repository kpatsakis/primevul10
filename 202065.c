static void ip6_tnl_add_linklocal(struct inet6_dev *idev)
{
	struct net_device *link_dev;

	/* first try to inherit the link-local address from the link device */
	if (idev->dev->iflink &&
	    (link_dev = __dev_get_by_index(idev->dev->iflink))) {
		if (!ipv6_inherit_linklocal(idev, link_dev))
			return;
	}
	/* then try to inherit it from any device */
	for (link_dev = dev_base; link_dev; link_dev = link_dev->next) {
		if (!ipv6_inherit_linklocal(idev, link_dev))
			return;
	}
	printk(KERN_DEBUG "init ip6-ip6: add_linklocal failed\n");
}