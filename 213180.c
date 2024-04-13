static bool type_is_pkt_pointer(enum bpf_reg_type type)
{
	return type == PTR_TO_PACKET ||
	       type == PTR_TO_PACKET_META;
}