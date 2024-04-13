static bool reg_type_may_be_null(enum bpf_reg_type type)
{
	return type == PTR_TO_MAP_VALUE_OR_NULL ||
	       type == PTR_TO_SOCKET_OR_NULL ||
	       type == PTR_TO_SOCK_COMMON_OR_NULL ||
	       type == PTR_TO_TCP_SOCK_OR_NULL;
}