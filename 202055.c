static int __init cbq_module_init(void)
{
	return register_qdisc(&cbq_qdisc_ops);
}