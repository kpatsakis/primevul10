static RFlagItem *core_flg_class_get(RFlag *f, const char *name) {
	r_flag_space_push (f, R_FLAGS_FS_CLASSES);
	RFlagItem *res = r_flag_get (f, name);
	r_flag_space_pop (f);
	return res;
}