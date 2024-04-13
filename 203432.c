static void __http_protocol_init(void)
{
	acl_register_keywords(&acl_kws);
	pattern_register_fetches(&pattern_fetch_keywords);
}