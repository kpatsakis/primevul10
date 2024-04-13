static int ethtool_get_rx_ntuple(struct net_device *dev, void __user *useraddr)
{
	struct ethtool_gstrings gstrings;
	const struct ethtool_ops *ops = dev->ethtool_ops;
	struct ethtool_rx_ntuple_flow_spec_container *fsc;
	u8 *data;
	char *p;
	int ret, i, num_strings = 0;

	if (!ops->get_sset_count)
		return -EOPNOTSUPP;

	if (copy_from_user(&gstrings, useraddr, sizeof(gstrings)))
		return -EFAULT;

	ret = ops->get_sset_count(dev, gstrings.string_set);
	if (ret < 0)
		return ret;

	gstrings.len = ret;

	data = kmalloc(gstrings.len * ETH_GSTRING_LEN, GFP_USER);
	if (!data)
		return -ENOMEM;

	if (ops->get_rx_ntuple) {
		/* driver-specific filter grab */
		ret = ops->get_rx_ntuple(dev, gstrings.string_set, data);
		goto copy;
	}

	/* default ethtool filter grab */
	i = 0;
	p = (char *)data;
	list_for_each_entry(fsc, &dev->ethtool_ntuple_list.list, list) {
		sprintf(p, "Filter %d:\n", i);
		p += ETH_GSTRING_LEN;
		num_strings++;

		switch (fsc->fs.flow_type) {
		case TCP_V4_FLOW:
			sprintf(p, "\tFlow Type: TCP\n");
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		case UDP_V4_FLOW:
			sprintf(p, "\tFlow Type: UDP\n");
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		case SCTP_V4_FLOW:
			sprintf(p, "\tFlow Type: SCTP\n");
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		case AH_ESP_V4_FLOW:
			sprintf(p, "\tFlow Type: AH ESP\n");
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		case ESP_V4_FLOW:
			sprintf(p, "\tFlow Type: ESP\n");
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		case IP_USER_FLOW:
			sprintf(p, "\tFlow Type: Raw IP\n");
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		case IPV4_FLOW:
			sprintf(p, "\tFlow Type: IPv4\n");
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		default:
			sprintf(p, "\tFlow Type: Unknown\n");
			p += ETH_GSTRING_LEN;
			num_strings++;
			goto unknown_filter;
		}

		/* now the rest of the filters */
		switch (fsc->fs.flow_type) {
		case TCP_V4_FLOW:
		case UDP_V4_FLOW:
		case SCTP_V4_FLOW:
			sprintf(p, "\tSrc IP addr: 0x%x\n",
				fsc->fs.h_u.tcp_ip4_spec.ip4src);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tSrc IP mask: 0x%x\n",
				fsc->fs.m_u.tcp_ip4_spec.ip4src);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tDest IP addr: 0x%x\n",
				fsc->fs.h_u.tcp_ip4_spec.ip4dst);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tDest IP mask: 0x%x\n",
				fsc->fs.m_u.tcp_ip4_spec.ip4dst);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tSrc Port: %d, mask: 0x%x\n",
				fsc->fs.h_u.tcp_ip4_spec.psrc,
				fsc->fs.m_u.tcp_ip4_spec.psrc);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tDest Port: %d, mask: 0x%x\n",
				fsc->fs.h_u.tcp_ip4_spec.pdst,
				fsc->fs.m_u.tcp_ip4_spec.pdst);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tTOS: %d, mask: 0x%x\n",
				fsc->fs.h_u.tcp_ip4_spec.tos,
				fsc->fs.m_u.tcp_ip4_spec.tos);
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		case AH_ESP_V4_FLOW:
		case ESP_V4_FLOW:
			sprintf(p, "\tSrc IP addr: 0x%x\n",
				fsc->fs.h_u.ah_ip4_spec.ip4src);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tSrc IP mask: 0x%x\n",
				fsc->fs.m_u.ah_ip4_spec.ip4src);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tDest IP addr: 0x%x\n",
				fsc->fs.h_u.ah_ip4_spec.ip4dst);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tDest IP mask: 0x%x\n",
				fsc->fs.m_u.ah_ip4_spec.ip4dst);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tSPI: %d, mask: 0x%x\n",
				fsc->fs.h_u.ah_ip4_spec.spi,
				fsc->fs.m_u.ah_ip4_spec.spi);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tTOS: %d, mask: 0x%x\n",
				fsc->fs.h_u.ah_ip4_spec.tos,
				fsc->fs.m_u.ah_ip4_spec.tos);
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		case IP_USER_FLOW:
			sprintf(p, "\tSrc IP addr: 0x%x\n",
				fsc->fs.h_u.raw_ip4_spec.ip4src);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tSrc IP mask: 0x%x\n",
				fsc->fs.m_u.raw_ip4_spec.ip4src);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tDest IP addr: 0x%x\n",
				fsc->fs.h_u.raw_ip4_spec.ip4dst);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tDest IP mask: 0x%x\n",
				fsc->fs.m_u.raw_ip4_spec.ip4dst);
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		case IPV4_FLOW:
			sprintf(p, "\tSrc IP addr: 0x%x\n",
				fsc->fs.h_u.usr_ip4_spec.ip4src);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tSrc IP mask: 0x%x\n",
				fsc->fs.m_u.usr_ip4_spec.ip4src);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tDest IP addr: 0x%x\n",
				fsc->fs.h_u.usr_ip4_spec.ip4dst);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tDest IP mask: 0x%x\n",
				fsc->fs.m_u.usr_ip4_spec.ip4dst);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tL4 bytes: 0x%x, mask: 0x%x\n",
				fsc->fs.h_u.usr_ip4_spec.l4_4_bytes,
				fsc->fs.m_u.usr_ip4_spec.l4_4_bytes);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tTOS: %d, mask: 0x%x\n",
				fsc->fs.h_u.usr_ip4_spec.tos,
				fsc->fs.m_u.usr_ip4_spec.tos);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tIP Version: %d, mask: 0x%x\n",
				fsc->fs.h_u.usr_ip4_spec.ip_ver,
				fsc->fs.m_u.usr_ip4_spec.ip_ver);
			p += ETH_GSTRING_LEN;
			num_strings++;
			sprintf(p, "\tProtocol: %d, mask: 0x%x\n",
				fsc->fs.h_u.usr_ip4_spec.proto,
				fsc->fs.m_u.usr_ip4_spec.proto);
			p += ETH_GSTRING_LEN;
			num_strings++;
			break;
		}
		sprintf(p, "\tVLAN: %d, mask: 0x%x\n",
			fsc->fs.vlan_tag, fsc->fs.vlan_tag_mask);
		p += ETH_GSTRING_LEN;
		num_strings++;
		sprintf(p, "\tUser-defined: 0x%Lx\n", fsc->fs.data);
		p += ETH_GSTRING_LEN;
		num_strings++;
		sprintf(p, "\tUser-defined mask: 0x%Lx\n", fsc->fs.data_mask);
		p += ETH_GSTRING_LEN;
		num_strings++;
		if (fsc->fs.action == ETHTOOL_RXNTUPLE_ACTION_DROP)
			sprintf(p, "\tAction: Drop\n");
		else
			sprintf(p, "\tAction: Direct to queue %d\n",
				fsc->fs.action);
		p += ETH_GSTRING_LEN;
		num_strings++;
unknown_filter:
		i++;
	}
copy:
	/* indicate to userspace how many strings we actually have */
	gstrings.len = num_strings;
	ret = -EFAULT;
	if (copy_to_user(useraddr, &gstrings, sizeof(gstrings)))
		goto out;
	useraddr += sizeof(gstrings);
	if (copy_to_user(useraddr, data, gstrings.len * ETH_GSTRING_LEN))
		goto out;
	ret = 0;

out:
	kfree(data);
	return ret;
}