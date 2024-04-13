static bool arg_type_is_int_ptr(enum bpf_arg_type type)
{
	return type == ARG_PTR_TO_INT ||
	       type == ARG_PTR_TO_LONG;
}