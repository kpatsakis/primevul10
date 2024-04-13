static int do_setlink(struct sk_buff *skb, struct nlmsghdr *nlh, void *arg)
{
	struct ifinfomsg  *ifm = NLMSG_DATA(nlh);
	struct rtattr    **ida = arg;
	struct net_device *dev;
	int err, send_addr_notify = 0;

	if (ifm->ifi_index >= 0)
		dev = dev_get_by_index(ifm->ifi_index);
	else if (ida[IFLA_IFNAME - 1]) {
		char ifname[IFNAMSIZ];

		if (rtattr_strlcpy(ifname, ida[IFLA_IFNAME - 1],
		                   IFNAMSIZ) >= IFNAMSIZ)
			return -EINVAL;
		dev = dev_get_by_name(ifname);
	} else
		return -EINVAL;

	if (!dev)
		return -ENODEV;

	err = -EINVAL;

	if (ifm->ifi_flags)
		dev_change_flags(dev, ifm->ifi_flags);

	if (ida[IFLA_MAP - 1]) {
		struct rtnl_link_ifmap *u_map;
		struct ifmap k_map;

		if (!dev->set_config) {
			err = -EOPNOTSUPP;
			goto out;
		}

		if (!netif_device_present(dev)) {
			err = -ENODEV;
			goto out;
		}
		
		if (ida[IFLA_MAP - 1]->rta_len != RTA_LENGTH(sizeof(*u_map)))
			goto out;

		u_map = RTA_DATA(ida[IFLA_MAP - 1]);

		k_map.mem_start = (unsigned long) u_map->mem_start;
		k_map.mem_end = (unsigned long) u_map->mem_end;
		k_map.base_addr = (unsigned short) u_map->base_addr;
		k_map.irq = (unsigned char) u_map->irq;
		k_map.dma = (unsigned char) u_map->dma;
		k_map.port = (unsigned char) u_map->port;

		err = dev->set_config(dev, &k_map);

		if (err)
			goto out;
	}

	if (ida[IFLA_ADDRESS - 1]) {
		if (!dev->set_mac_address) {
			err = -EOPNOTSUPP;
			goto out;
		}
		if (!netif_device_present(dev)) {
			err = -ENODEV;
			goto out;
		}
		if (ida[IFLA_ADDRESS - 1]->rta_len != RTA_LENGTH(dev->addr_len))
			goto out;

		err = dev->set_mac_address(dev, RTA_DATA(ida[IFLA_ADDRESS - 1]));
		if (err)
			goto out;
		send_addr_notify = 1;
	}

	if (ida[IFLA_BROADCAST - 1]) {
		if (ida[IFLA_BROADCAST - 1]->rta_len != RTA_LENGTH(dev->addr_len))
			goto out;
		memcpy(dev->broadcast, RTA_DATA(ida[IFLA_BROADCAST - 1]),
		       dev->addr_len);
		send_addr_notify = 1;
	}

	if (ida[IFLA_MTU - 1]) {
		if (ida[IFLA_MTU - 1]->rta_len != RTA_LENGTH(sizeof(u32)))
			goto out;
		err = dev_set_mtu(dev, *((u32 *) RTA_DATA(ida[IFLA_MTU - 1])));

		if (err)
			goto out;

	}

	if (ida[IFLA_TXQLEN - 1]) {
		if (ida[IFLA_TXQLEN - 1]->rta_len != RTA_LENGTH(sizeof(u32)))
			goto out;

		dev->tx_queue_len = *((u32 *) RTA_DATA(ida[IFLA_TXQLEN - 1]));
	}

	if (ida[IFLA_WEIGHT - 1]) {
		if (ida[IFLA_WEIGHT - 1]->rta_len != RTA_LENGTH(sizeof(u32)))
			goto out;

		dev->weight = *((u32 *) RTA_DATA(ida[IFLA_WEIGHT - 1]));
	}

	if (ifm->ifi_index >= 0 && ida[IFLA_IFNAME - 1]) {
		char ifname[IFNAMSIZ];

		if (rtattr_strlcpy(ifname, ida[IFLA_IFNAME - 1],
		                   IFNAMSIZ) >= IFNAMSIZ)
			goto out;
		err = dev_change_name(dev, ifname);
		if (err)
			goto out;
	}

	err = 0;

out:
	if (send_addr_notify)
		call_netdevice_notifiers(NETDEV_CHANGEADDR, dev);

	dev_put(dev);
	return err;
}