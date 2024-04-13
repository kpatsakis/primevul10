create_token_tree (re_dfa_t *dfa, bin_tree_t *left, bin_tree_t *right,
		   const re_token_t *token)
{
  bin_tree_t *tree;
  if (BE (dfa->str_tree_storage_idx == BIN_TREE_STORAGE_SIZE, 0))
    {
      bin_tree_storage_t *storage = re_malloc (bin_tree_storage_t, 1);

      if (storage == NULL)
	return NULL;
      storage->next = dfa->str_tree_storage;
      dfa->str_tree_storage = storage;
      dfa->str_tree_storage_idx = 0;
    }
  tree = &dfa->str_tree_storage->data[dfa->str_tree_storage_idx++];

  tree->parent = NULL;
  tree->left = left;
  tree->right = right;
  tree->token = *token;
  tree->token.duplicated = 0;
  tree->token.opt_subexp = 0;
  tree->first = NULL;
  tree->next = NULL;
  tree->node_idx = REG_MISSING;

  if (left != NULL)
    left->parent = tree;
  if (right != NULL)
    right->parent = tree;
  return tree;
}