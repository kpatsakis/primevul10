calc_first (void *extra, bin_tree_t *node)
{
  re_dfa_t *dfa = (re_dfa_t *) extra;
  if (node->token.type == CONCAT)
    {
      node->first = node->left->first;
      node->node_idx = node->left->node_idx;
    }
  else
    {
      node->first = node;
      node->node_idx = re_dfa_add_node (dfa, node->token);
      if (BE (node->node_idx == REG_MISSING, 0))
	return REG_ESPACE;
      if (node->token.type == ANCHOR)
	dfa->nodes[node->node_idx].constraint = node->token.opr.ctx_type;
    }
  return REG_NOERROR;
}