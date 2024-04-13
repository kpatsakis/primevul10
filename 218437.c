calc_inveclosure (re_dfa_t *dfa)
{
  Idx src, idx;
  bool ok;
  for (idx = 0; idx < dfa->nodes_len; ++idx)
    re_node_set_init_empty (dfa->inveclosures + idx);

  for (src = 0; src < dfa->nodes_len; ++src)
    {
      Idx *elems = dfa->eclosures[src].elems;
      for (idx = 0; idx < dfa->eclosures[src].nelem; ++idx)
	{
	  ok = re_node_set_insert_last (dfa->inveclosures + elems[idx], src);
	  if (BE (! ok, 0))
	    return REG_ESPACE;
	}
    }

  return REG_NOERROR;
}