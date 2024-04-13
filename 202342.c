static int vif_delete(int vifi)
{
	struct vif_device *v;
	struct net_device *dev;
	struct in_device *in_dev;

	if (vifi < 0 || vifi >= maxvif)
		return -EADDRNOTAVAIL;

	v = &vif_table[vifi];

	write_lock_bh(&mrt_lock);
	dev = v->dev;
	v->dev = NULL;

	if (!dev) {
		write_unlock_bh(&mrt_lock);
		return -EADDRNOTAVAIL;
	}

#ifdef CONFIG_IP_PIMSM
	if (vifi == reg_vif_num)
		reg_vif_num = -1;
#endif

	if (vifi+1 == maxvif) {
		int tmp;
		for (tmp=vifi-1; tmp>=0; tmp--) {
			if (VIF_EXISTS(tmp))
				break;
		}
		maxvif = tmp+1;
	}

	write_unlock_bh(&mrt_lock);

	dev_set_allmulti(dev, -1);

	if ((in_dev = __in_dev_get(dev)) != NULL) {
		in_dev->cnf.mc_forwarding--;
		ip_rt_multicast_event(in_dev);
	}

	if (v->flags&(VIFF_TUNNEL|VIFF_REGISTER))
		unregister_netdevice(dev);

	dev_put(dev);
	return 0;
}