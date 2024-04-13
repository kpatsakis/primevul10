static uint8_t pfkey_proto_to_xfrm(uint8_t proto)
{
	return proto == IPSEC_PROTO_ANY ? 0 : proto;
}