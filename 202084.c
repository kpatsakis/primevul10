static void __exit cbq_module_exit(void) 
{
	unregister_qdisc(&cbq_qdisc_ops);
}