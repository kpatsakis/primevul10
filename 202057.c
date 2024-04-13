int __init if6_proc_init(void)
{
	if (!proc_net_fops_create("if_inet6", S_IRUGO, &if6_fops))
		return -ENOMEM;
	return 0;
}