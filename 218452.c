search_duplicated_node (const re_dfa_t *dfa, Idx org_node,
			unsigned int constraint)
{
  Idx idx;
  for (idx = dfa->nodes_len - 1; dfa->nodes[idx].duplicated && idx > 0; --idx)
    {
      if (org_node == dfa->org_indices[idx]
	  && constraint == dfa->nodes[idx].constraint)
	return idx; /* Found.  */
    }
  return REG_MISSING; /* Not found.  */
}