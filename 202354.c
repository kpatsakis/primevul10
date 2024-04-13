int ip_mroute_setsockopt(struct sock *sk,int optname,char __user *optval,int optlen)
{
	int ret;
	struct vifctl vif;
	struct mfcctl mfc;
	
	if(optname!=MRT_INIT)
	{
		if(sk!=mroute_socket && !capable(CAP_NET_ADMIN))
			return -EACCES;
	}

	switch(optname)
	{
		case MRT_INIT:
			if (sk->sk_type != SOCK_RAW ||
			    inet_sk(sk)->num != IPPROTO_IGMP)
				return -EOPNOTSUPP;
			if(optlen!=sizeof(int))
				return -ENOPROTOOPT;

			rtnl_lock();
			if (mroute_socket) {
				rtnl_unlock();
				return -EADDRINUSE;
			}

			ret = ip_ra_control(sk, 1, mrtsock_destruct);
			if (ret == 0) {
				write_lock_bh(&mrt_lock);
				mroute_socket=sk;
				write_unlock_bh(&mrt_lock);

				ipv4_devconf.mc_forwarding++;
			}
			rtnl_unlock();
			return ret;
		case MRT_DONE:
			if (sk!=mroute_socket)
				return -EACCES;
			return ip_ra_control(sk, 0, NULL);
		case MRT_ADD_VIF:
		case MRT_DEL_VIF:
			if(optlen!=sizeof(vif))
				return -EINVAL;
			if (copy_from_user(&vif,optval,sizeof(vif)))
				return -EFAULT; 
			if(vif.vifc_vifi >= MAXVIFS)
				return -ENFILE;
			rtnl_lock();
			if (optname==MRT_ADD_VIF) {
				ret = vif_add(&vif, sk==mroute_socket);
			} else {
				ret = vif_delete(vif.vifc_vifi);
			}
			rtnl_unlock();
			return ret;

		/*
		 *	Manipulate the forwarding caches. These live
		 *	in a sort of kernel/user symbiosis.
		 */
		case MRT_ADD_MFC:
		case MRT_DEL_MFC:
			if(optlen!=sizeof(mfc))
				return -EINVAL;
			if (copy_from_user(&mfc,optval, sizeof(mfc)))
				return -EFAULT;
			rtnl_lock();
			if (optname==MRT_DEL_MFC)
				ret = ipmr_mfc_delete(&mfc);
			else
				ret = ipmr_mfc_add(&mfc, sk==mroute_socket);
			rtnl_unlock();
			return ret;
		/*
		 *	Control PIM assert.
		 */
		case MRT_ASSERT:
		{
			int v;
			if(get_user(v,(int __user *)optval))
				return -EFAULT;
			mroute_do_assert=(v)?1:0;
			return 0;
		}
#ifdef CONFIG_IP_PIMSM
		case MRT_PIM:
		{
			int v, ret;
			if(get_user(v,(int __user *)optval))
				return -EFAULT;
			v = (v)?1:0;
			rtnl_lock();
			ret = 0;
			if (v != mroute_do_pim) {
				mroute_do_pim = v;
				mroute_do_assert = v;
#ifdef CONFIG_IP_PIMSM_V2
				if (mroute_do_pim)
					ret = inet_add_protocol(&pim_protocol,
								IPPROTO_PIM);
				else
					ret = inet_del_protocol(&pim_protocol,
								IPPROTO_PIM);
				if (ret < 0)
					ret = -EAGAIN;
#endif
			}
			rtnl_unlock();
			return ret;
		}
#endif
		/*
		 *	Spurious command, or MRT_VERSION which you cannot
		 *	set.
		 */
		default:
			return -ENOPROTOOPT;
	}
}