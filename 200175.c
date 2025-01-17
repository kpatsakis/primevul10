static int __init sit_init(void)
{
	int err;

	printk(KERN_INFO "IPv6 over IPv4 tunneling driver\n");

	if (xfrm4_tunnel_register(&sit_handler, AF_INET6) < 0) {
		printk(KERN_INFO "sit init: Can't add protocol\n");
		return -EAGAIN;
	}

	err = register_pernet_gen_device(&sit_net_id, &sit_net_ops);
	if (err < 0)
		xfrm4_tunnel_deregister(&sit_handler, AF_INET6);

	return err;
}