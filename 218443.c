free_tree (void *extra, bin_tree_t *node)
{
  free_token (&node->token);
  return REG_NOERROR;
}