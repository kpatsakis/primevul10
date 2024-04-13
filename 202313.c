int __init wireless_proc_init(void)
{
	if (!proc_net_fops_create("wireless", S_IRUGO, &wireless_seq_fops))
		return -ENOMEM;

	return 0;
}