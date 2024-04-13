const char *kernel_type_name(u32 id)
{
	return btf_name_by_offset(btf_vmlinux,
				  btf_type_by_id(btf_vmlinux, id)->name_off);
}