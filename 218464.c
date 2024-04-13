parse_expression (re_string_t *regexp, regex_t *preg, re_token_t *token,
		  reg_syntax_t syntax, Idx nest, reg_errcode_t *err)
{
  re_dfa_t *dfa = preg->buffer;
  bin_tree_t *tree;
  switch (token->type)
    {
    case CHARACTER:
      tree = create_token_tree (dfa, NULL, NULL, token);
      if (BE (tree == NULL, 0))
	{
	  *err = REG_ESPACE;
	  return NULL;
	}
#ifdef RE_ENABLE_I18N
      if (dfa->mb_cur_max > 1)
	{
	  while (!re_string_eoi (regexp)
		 && !re_string_first_byte (regexp, re_string_cur_idx (regexp)))
	    {
	      bin_tree_t *mbc_remain;
	      fetch_token (token, regexp, syntax);
	      mbc_remain = create_token_tree (dfa, NULL, NULL, token);
	      tree = create_tree (dfa, tree, mbc_remain, CONCAT);
	      if (BE (mbc_remain == NULL || tree == NULL, 0))
		{
		  *err = REG_ESPACE;
		  return NULL;
		}
	    }
	}
#endif
      break;
    case OP_OPEN_SUBEXP:
      tree = parse_sub_exp (regexp, preg, token, syntax, nest + 1, err);
      if (BE (*err != REG_NOERROR && tree == NULL, 0))
	return NULL;
      break;
    case OP_OPEN_BRACKET:
      tree = parse_bracket_exp (regexp, dfa, token, syntax, err);
      if (BE (*err != REG_NOERROR && tree == NULL, 0))
	return NULL;
      break;
    case OP_BACK_REF:
      if (!BE (dfa->completed_bkref_map & (1 << token->opr.idx), 1))
	{
	  *err = REG_ESUBREG;
	  return NULL;
	}
      dfa->used_bkref_map |= 1 << token->opr.idx;
      tree = create_token_tree (dfa, NULL, NULL, token);
      if (BE (tree == NULL, 0))
	{
	  *err = REG_ESPACE;
	  return NULL;
	}
      ++dfa->nbackref;
      dfa->has_mb_node = 1;
      break;
    case OP_OPEN_DUP_NUM:
      if (syntax & RE_CONTEXT_INVALID_DUP)
	{
	  *err = REG_BADRPT;
	  return NULL;
	}
      /* FALLTHROUGH */
    case OP_DUP_ASTERISK:
    case OP_DUP_PLUS:
    case OP_DUP_QUESTION:
      if (syntax & RE_CONTEXT_INVALID_OPS)
	{
	  *err = REG_BADRPT;
	  return NULL;
	}
      else if (syntax & RE_CONTEXT_INDEP_OPS)
	{
	  fetch_token (token, regexp, syntax);
	  return parse_expression (regexp, preg, token, syntax, nest, err);
	}
      /* else fall through  */
    case OP_CLOSE_SUBEXP:
      if ((token->type == OP_CLOSE_SUBEXP) &&
	  !(syntax & RE_UNMATCHED_RIGHT_PAREN_ORD))
	{
	  *err = REG_ERPAREN;
	  return NULL;
	}
      /* else fall through  */
    case OP_CLOSE_DUP_NUM:
      /* We treat it as a normal character.  */

      /* Then we can these characters as normal characters.  */
      token->type = CHARACTER;
      /* mb_partial and word_char bits should be initialized already
	 by peek_token.  */
      tree = create_token_tree (dfa, NULL, NULL, token);
      if (BE (tree == NULL, 0))
	{
	  *err = REG_ESPACE;
	  return NULL;
	}
      break;
    case ANCHOR:
      if ((token->opr.ctx_type
	   & (WORD_DELIM | NOT_WORD_DELIM | WORD_FIRST | WORD_LAST))
	  && dfa->word_ops_used == 0)
	init_word_char (dfa);
      if (token->opr.ctx_type == WORD_DELIM
	  || token->opr.ctx_type == NOT_WORD_DELIM)
	{
	  bin_tree_t *tree_first, *tree_last;
	  if (token->opr.ctx_type == WORD_DELIM)
	    {
	      token->opr.ctx_type = WORD_FIRST;
	      tree_first = create_token_tree (dfa, NULL, NULL, token);
	      token->opr.ctx_type = WORD_LAST;
	    }
	  else
	    {
	      token->opr.ctx_type = INSIDE_WORD;
	      tree_first = create_token_tree (dfa, NULL, NULL, token);
	      token->opr.ctx_type = INSIDE_NOTWORD;
	    }
	  tree_last = create_token_tree (dfa, NULL, NULL, token);
	  tree = create_tree (dfa, tree_first, tree_last, OP_ALT);
	  if (BE (tree_first == NULL || tree_last == NULL || tree == NULL, 0))
	    {
	      *err = REG_ESPACE;
	      return NULL;
	    }
	}
      else
	{
	  tree = create_token_tree (dfa, NULL, NULL, token);
	  if (BE (tree == NULL, 0))
	    {
	      *err = REG_ESPACE;
	      return NULL;
	    }
	}
      /* We must return here, since ANCHORs can't be followed
	 by repetition operators.
	 eg. RE"^*" is invalid or "<ANCHOR(^)><CHAR(*)>",
	     it must not be "<ANCHOR(^)><REPEAT(*)>".  */
      fetch_token (token, regexp, syntax);
      return tree;
    case OP_PERIOD:
      tree = create_token_tree (dfa, NULL, NULL, token);
      if (BE (tree == NULL, 0))
	{
	  *err = REG_ESPACE;
	  return NULL;
	}
      if (dfa->mb_cur_max > 1)
	dfa->has_mb_node = 1;
      break;
    case OP_WORD:
    case OP_NOTWORD:
      tree = build_charclass_op (dfa, regexp->trans,
				 "alnum",
				 "_",
				 token->type == OP_NOTWORD, err);
      if (BE (*err != REG_NOERROR && tree == NULL, 0))
	return NULL;
      break;
    case OP_SPACE:
    case OP_NOTSPACE:
      tree = build_charclass_op (dfa, regexp->trans,
				 "space",
				 "",
				 token->type == OP_NOTSPACE, err);
      if (BE (*err != REG_NOERROR && tree == NULL, 0))
	return NULL;
      break;
    case OP_ALT:
    case END_OF_RE:
      return NULL;
    case BACK_SLASH:
      *err = REG_EESCAPE;
      return NULL;
    default:
      /* Must not happen?  */
#ifdef DEBUG
      assert (0);
#endif
      return NULL;
    }
  fetch_token (token, regexp, syntax);

  while (token->type == OP_DUP_ASTERISK || token->type == OP_DUP_PLUS
	 || token->type == OP_DUP_QUESTION || token->type == OP_OPEN_DUP_NUM)
    {
      bin_tree_t *dup_tree = parse_dup_op (tree, regexp, dfa, token,
					   syntax, err);
      if (BE (*err != REG_NOERROR && dup_tree == NULL, 0))
	{
	  if (tree != NULL)
	    postorder (tree, free_tree, NULL);
	  return NULL;
	}
      tree = dup_tree;
      /* In BRE consecutive duplications are not allowed.  */
      if ((syntax & RE_CONTEXT_INVALID_DUP)
	  && (token->type == OP_DUP_ASTERISK
	      || token->type == OP_OPEN_DUP_NUM))
	{
	  if (tree != NULL)
	    postorder (tree, free_tree, NULL);
	  *err = REG_BADRPT;
	  return NULL;
	}
    }

  return tree;
}