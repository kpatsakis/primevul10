create_backup_copy (char const *from, char const *to, struct stat *st,
		    bool to_dir_known_to_exist, bool remember_backup)
{
  struct stat backup_st;
  copy_file (from, to, remember_backup ? &backup_st : NULL, 0, st->st_mode,
	     to_dir_known_to_exist);
  if (remember_backup)
    insert_file (&backup_st);
  set_file_attributes (to, FA_TIMES | FA_IDS | FA_MODE, from, st, st->st_mode, NULL);
}