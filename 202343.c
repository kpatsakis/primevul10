struct net_device *ipmr_new_tunnel(struct vifctl *v)
{
	struct net_device  *dev;

	dev = __dev_get_by_name("tunl0");

	if (dev) {
		int err;
		struct ifreq ifr;
		mm_segment_t	oldfs;
		struct ip_tunnel_parm p;
		struct in_device  *in_dev;

		memset(&p, 0, sizeof(p));
		p.iph.daddr = v->vifc_rmt_addr.s_addr;
		p.iph.saddr = v->vifc_lcl_addr.s_addr;
		p.iph.version = 4;
		p.iph.ihl = 5;
		p.iph.protocol = IPPROTO_IPIP;
		sprintf(p.name, "dvmrp%d", v->vifc_vifi);
		ifr.ifr_ifru.ifru_data = (void*)&p;

		oldfs = get_fs(); set_fs(KERNEL_DS);
		err = dev->do_ioctl(dev, &ifr, SIOCADDTUNNEL);
		set_fs(oldfs);

		dev = NULL;

		if (err == 0 && (dev = __dev_get_by_name(p.name)) != NULL) {
			dev->flags |= IFF_MULTICAST;

			in_dev = __in_dev_get(dev);
			if (in_dev == NULL && (in_dev = inetdev_init(dev)) == NULL)
				goto failure;
			in_dev->cnf.rp_filter = 0;

			if (dev_open(dev))
				goto failure;
		}
	}
	return dev;

failure:
	/* allow the register to be completed before unregistering. */
	rtnl_unlock();
	rtnl_lock();

	unregister_netdevice(dev);
	return NULL;
}