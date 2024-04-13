create_tree (re_dfa_t *dfa, bin_tree_t *left, bin_tree_t *right,
	     re_token_type_t type)
{
  re_token_t t;
  t.type = type;
  return create_token_tree (dfa, left, right, &t);
}