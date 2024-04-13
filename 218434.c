parse_branch (re_string_t *regexp, regex_t *preg, re_token_t *token,
	      reg_syntax_t syntax, Idx nest, reg_errcode_t *err)
{
  bin_tree_t *tree, *expr;
  re_dfa_t *dfa = preg->buffer;
  tree = parse_expression (regexp, preg, token, syntax, nest, err);
  if (BE (*err != REG_NOERROR && tree == NULL, 0))
    return NULL;

  while (token->type != OP_ALT && token->type != END_OF_RE
	 && (nest == 0 || token->type != OP_CLOSE_SUBEXP))
    {
      expr = parse_expression (regexp, preg, token, syntax, nest, err);
      if (BE (*err != REG_NOERROR && expr == NULL, 0))
	{
	  if (tree != NULL)
	    postorder (tree, free_tree, NULL);
	  return NULL;
	}
      if (tree != NULL && expr != NULL)
	{
	  bin_tree_t *newtree = create_tree (dfa, tree, expr, CONCAT);
	  if (newtree == NULL)
	    {
	      postorder (expr, free_tree, NULL);
	      postorder (tree, free_tree, NULL);
	      *err = REG_ESPACE;
	      return NULL;
	    }
	  tree = newtree;
	}
      else if (tree == NULL)
	tree = expr;
      /* Otherwise expr == NULL, we don't need to create new tree.  */
    }
  return tree;
}