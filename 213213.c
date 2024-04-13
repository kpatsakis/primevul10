static bool arg_type_may_be_refcounted(enum bpf_arg_type type)
{
	return type == ARG_PTR_TO_SOCK_COMMON;
}