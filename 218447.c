free_dfa_content (re_dfa_t *dfa)
{
  Idx i, j;

  if (dfa->nodes)
    for (i = 0; i < dfa->nodes_len; ++i)
      free_token (dfa->nodes + i);
  re_free (dfa->nexts);
  for (i = 0; i < dfa->nodes_len; ++i)
    {
      if (dfa->eclosures != NULL)
	re_node_set_free (dfa->eclosures + i);
      if (dfa->inveclosures != NULL)
	re_node_set_free (dfa->inveclosures + i);
      if (dfa->edests != NULL)
	re_node_set_free (dfa->edests + i);
    }
  re_free (dfa->edests);
  re_free (dfa->eclosures);
  re_free (dfa->inveclosures);
  re_free (dfa->nodes);

  if (dfa->state_table)
    for (i = 0; i <= dfa->state_hash_mask; ++i)
      {
	struct re_state_table_entry *entry = dfa->state_table + i;
	for (j = 0; j < entry->num; ++j)
	  {
	    re_dfastate_t *state = entry->array[j];
	    free_state (state);
	  }
	re_free (entry->array);
      }
  re_free (dfa->state_table);
#ifdef RE_ENABLE_I18N
  if (dfa->sb_char != utf8_sb_map)
    re_free (dfa->sb_char);
#endif
  re_free (dfa->subexp_map);
#ifdef DEBUG
  re_free (dfa->re_str);
#endif

  re_free (dfa);
}