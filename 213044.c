static RFlagItem *core_flg_class_set(RFlag *f, const char *name, ut64 addr, ut32 size) {
	r_flag_space_push (f, R_FLAGS_FS_CLASSES);
	RFlagItem *res = r_flag_set (f, name, addr, size);
	r_flag_space_pop (f);
	return res;
}