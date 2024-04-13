static int __init ipsec_pfkey_init(void)
{
	int err = proto_register(&key_proto, 0);

	if (err != 0)
		goto out;

	err = register_pernet_subsys(&pfkey_net_ops);
	if (err != 0)
		goto out_unregister_key_proto;
	err = sock_register(&pfkey_family_ops);
	if (err != 0)
		goto out_unregister_pernet;
	err = xfrm_register_km(&pfkeyv2_mgr);
	if (err != 0)
		goto out_sock_unregister;
out:
	return err;

out_sock_unregister:
	sock_unregister(PF_KEY);
out_unregister_pernet:
	unregister_pernet_subsys(&pfkey_net_ops);
out_unregister_key_proto:
	proto_unregister(&key_proto);
	goto out;
}