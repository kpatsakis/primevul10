static bool reg_type_may_be_refcounted_or_null(enum bpf_reg_type type)
{
	return type == PTR_TO_SOCKET ||
		type == PTR_TO_SOCKET_OR_NULL ||
		type == PTR_TO_TCP_SOCK ||
		type == PTR_TO_TCP_SOCK_OR_NULL;
}