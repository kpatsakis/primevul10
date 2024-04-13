analyze (regex_t *preg)
{
  re_dfa_t *dfa = preg->buffer;
  reg_errcode_t ret;

  /* Allocate arrays.  */
  dfa->nexts = re_malloc (Idx, dfa->nodes_alloc);
  dfa->org_indices = re_malloc (Idx, dfa->nodes_alloc);
  dfa->edests = re_malloc (re_node_set, dfa->nodes_alloc);
  dfa->eclosures = re_malloc (re_node_set, dfa->nodes_alloc);
  if (BE (dfa->nexts == NULL || dfa->org_indices == NULL || dfa->edests == NULL
	  || dfa->eclosures == NULL, 0))
    return REG_ESPACE;

  dfa->subexp_map = re_malloc (Idx, preg->re_nsub);
  if (dfa->subexp_map != NULL)
    {
      Idx i;
      for (i = 0; i < preg->re_nsub; i++)
	dfa->subexp_map[i] = i;
      preorder (dfa->str_tree, optimize_subexps, dfa);
      for (i = 0; i < preg->re_nsub; i++)
	if (dfa->subexp_map[i] != i)
	  break;
      if (i == preg->re_nsub)
	{
	  free (dfa->subexp_map);
	  dfa->subexp_map = NULL;
	}
    }

  ret = postorder (dfa->str_tree, lower_subexps, preg);
  if (BE (ret != REG_NOERROR, 0))
    return ret;
  ret = postorder (dfa->str_tree, calc_first, dfa);
  if (BE (ret != REG_NOERROR, 0))
    return ret;
  preorder (dfa->str_tree, calc_next, dfa);
  ret = preorder (dfa->str_tree, link_nfa_nodes, dfa);
  if (BE (ret != REG_NOERROR, 0))
    return ret;
  ret = calc_eclosure (dfa);
  if (BE (ret != REG_NOERROR, 0))
    return ret;

  /* We only need this during the prune_impossible_nodes pass in regexec.c;
     skip it if p_i_n will not run, as calc_inveclosure can be quadratic.  */
  if ((!preg->no_sub && preg->re_nsub > 0 && dfa->has_plural_match)
      || dfa->nbackref)
    {
      dfa->inveclosures = re_malloc (re_node_set, dfa->nodes_len);
      if (BE (dfa->inveclosures == NULL, 0))
	return REG_ESPACE;
      ret = calc_inveclosure (dfa);
    }

  return ret;
}