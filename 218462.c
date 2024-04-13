duplicate_tree (const bin_tree_t *root, re_dfa_t *dfa)
{
  const bin_tree_t *node;
  bin_tree_t *dup_root;
  bin_tree_t **p_new = &dup_root, *dup_node = root->parent;

  for (node = root; ; )
    {
      /* Create a new tree and link it back to the current parent.  */
      *p_new = create_token_tree (dfa, NULL, NULL, &node->token);
      if (*p_new == NULL)
	return NULL;
      (*p_new)->parent = dup_node;
      (*p_new)->token.duplicated = 1;
      dup_node = *p_new;

      /* Go to the left node, or up and to the right.  */
      if (node->left)
	{
	  node = node->left;
	  p_new = &dup_node->left;
	}
      else
	{
	  const bin_tree_t *prev = NULL;
	  while (node->right == prev || node->right == NULL)
	    {
	      prev = node;
	      node = node->parent;
	      dup_node = dup_node->parent;
	      if (!node)
		return dup_root;
	    }
	  node = node->right;
	  p_new = &dup_node->right;
	}
    }
}