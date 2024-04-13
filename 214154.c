static void reset_file_context(struct rar5* rar) {
	memset(&rar->file, 0, sizeof(rar->file));
	blake2sp_init(&rar->file.b2state, 32);

	if(rar->main.solid) {
		rar->cstate.solid_offset += rar->cstate.write_ptr;
	} else {
		rar->cstate.solid_offset = 0;
	}

	rar->cstate.write_ptr = 0;
	rar->cstate.last_write_ptr = 0;
	rar->cstate.last_unstore_ptr = 0;

	rar->file.redir_type = REDIR_TYPE_NONE;
	rar->file.redir_flags = 0;

	free_filters(rar);
}