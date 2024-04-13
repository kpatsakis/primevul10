int ip_mroute_getsockopt(struct sock *sk,int optname,char __user *optval,int __user *optlen)
{
	int olr;
	int val;

	if(optname!=MRT_VERSION && 
#ifdef CONFIG_IP_PIMSM
	   optname!=MRT_PIM &&
#endif
	   optname!=MRT_ASSERT)
		return -ENOPROTOOPT;

	if (get_user(olr, optlen))
		return -EFAULT;

	olr = min_t(unsigned int, olr, sizeof(int));
	if (olr < 0)
		return -EINVAL;
		
	if(put_user(olr,optlen))
		return -EFAULT;
	if(optname==MRT_VERSION)
		val=0x0305;
#ifdef CONFIG_IP_PIMSM
	else if(optname==MRT_PIM)
		val=mroute_do_pim;
#endif
	else
		val=mroute_do_assert;
	if(copy_to_user(optval,&val,olr))
		return -EFAULT;
	return 0;
}