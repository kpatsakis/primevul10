static bool exists_var(RPrint *print, ut64 func_addr, char *str) {
	char *name_key = sdb_fmt ("var.0x%"PFMT64x ".%d.%s", func_addr, 1, str);
	if (sdb_const_get_len (((RCore*)(print->user))->anal->sdb_fcns, name_key, NULL, 0)) {
		return true;
	}
	return false;
}