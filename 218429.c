init_dfa (re_dfa_t *dfa, size_t pat_len)
{
  __re_size_t table_size;
#ifndef _LIBC
  const char *codeset_name;
#endif
#ifdef RE_ENABLE_I18N
  size_t max_i18n_object_size = MAX (sizeof (wchar_t), sizeof (wctype_t));
#else
  size_t max_i18n_object_size = 0;
#endif
  size_t max_object_size =
    MAX (sizeof (struct re_state_table_entry),
	 MAX (sizeof (re_token_t),
	      MAX (sizeof (re_node_set),
		   MAX (sizeof (regmatch_t),
			max_i18n_object_size))));

  memset (dfa, '\0', sizeof (re_dfa_t));

  /* Force allocation of str_tree_storage the first time.  */
  dfa->str_tree_storage_idx = BIN_TREE_STORAGE_SIZE;

  /* Avoid overflows.  The extra "/ 2" is for the table_size doubling
     calculation below, and for similar doubling calculations
     elsewhere.  And it's <= rather than <, because some of the
     doubling calculations add 1 afterwards.  */
  if (BE (MIN (IDX_MAX, SIZE_MAX / max_object_size) / 2 <= pat_len, 0))
    return REG_ESPACE;

  dfa->nodes_alloc = pat_len + 1;
  dfa->nodes = re_malloc (re_token_t, dfa->nodes_alloc);

  /*  table_size = 2 ^ ceil(log pat_len) */
  for (table_size = 1; ; table_size <<= 1)
    if (table_size > pat_len)
      break;

  dfa->state_table = calloc (sizeof (struct re_state_table_entry), table_size);
  dfa->state_hash_mask = table_size - 1;

  dfa->mb_cur_max = MB_CUR_MAX;
#ifdef _LIBC
  if (dfa->mb_cur_max == 6
      && strcmp (_NL_CURRENT (LC_CTYPE, _NL_CTYPE_CODESET_NAME), "UTF-8") == 0)
    dfa->is_utf8 = 1;
  dfa->map_notascii = (_NL_CURRENT_WORD (LC_CTYPE, _NL_CTYPE_MAP_TO_NONASCII)
		       != 0);
#else
  codeset_name = nl_langinfo (CODESET);
  if ((codeset_name[0] == 'U' || codeset_name[0] == 'u')
      && (codeset_name[1] == 'T' || codeset_name[1] == 't')
      && (codeset_name[2] == 'F' || codeset_name[2] == 'f')
      && strcmp (codeset_name + 3 + (codeset_name[3] == '-'), "8") == 0)
    dfa->is_utf8 = 1;

  /* We check exhaustively in the loop below if this charset is a
     superset of ASCII.  */
  dfa->map_notascii = 0;
#endif

#ifdef RE_ENABLE_I18N
  if (dfa->mb_cur_max > 1)
    {
      if (dfa->is_utf8)
	dfa->sb_char = (re_bitset_ptr_t) utf8_sb_map;
      else
	{
	  int i, j, ch;

	  dfa->sb_char = (re_bitset_ptr_t) calloc (sizeof (bitset_t), 1);
	  if (BE (dfa->sb_char == NULL, 0))
	    return REG_ESPACE;

	  /* Set the bits corresponding to single byte chars.  */
	  for (i = 0, ch = 0; i < BITSET_WORDS; ++i)
	    for (j = 0; j < BITSET_WORD_BITS; ++j, ++ch)
	      {
		wint_t wch = __btowc (ch);
		if (wch != WEOF)
		  dfa->sb_char[i] |= (bitset_word_t) 1 << j;
# ifndef _LIBC
		if (isascii (ch) && wch != ch)
		  dfa->map_notascii = 1;
# endif
	      }
	}
    }
#endif

  if (BE (dfa->nodes == NULL || dfa->state_table == NULL, 0))
    return REG_ESPACE;
  return REG_NOERROR;
}