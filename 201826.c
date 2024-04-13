void udp6_proc_exit(void) {
	udp_proc_unregister(&udp6_seq_afinfo);
}