static int __net_init pfkey_init_proc(struct net *net)
{
	struct proc_dir_entry *e;

	e = proc_create_net("pfkey", 0, net->proc_net, &pfkey_seq_ops,
			sizeof(struct seq_net_private));
	if (e == NULL)
		return -ENOMEM;

	return 0;
}