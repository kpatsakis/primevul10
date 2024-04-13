init_backup_hash_table (void)
{
  file_id_table = hash_initialize (0, NULL, file_id_hasher,
				   file_id_comparator, free);
  if (!file_id_table)
    xalloc_die ();
}