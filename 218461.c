mark_opt_subexp (void *extra, bin_tree_t *node)
{
  Idx idx = (uintptr_t) extra;
  if (node->token.type == SUBEXP && node->token.opr.idx == idx)
    node->token.opt_subexp = 1;

  return REG_NOERROR;
}