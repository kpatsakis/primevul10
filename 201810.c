int __init udp6_proc_init(void)
{
	return udp_proc_register(&udp6_seq_afinfo);
}