optimize_utf8 (re_dfa_t *dfa)
{
  Idx node;
  int i;
  bool mb_chars = false;
  bool has_period = false;

  for (node = 0; node < dfa->nodes_len; ++node)
    switch (dfa->nodes[node].type)
      {
      case CHARACTER:
	if (dfa->nodes[node].opr.c >= ASCII_CHARS)
	  mb_chars = true;
	break;
      case ANCHOR:
	switch (dfa->nodes[node].opr.ctx_type)
	  {
	  case LINE_FIRST:
	  case LINE_LAST:
	  case BUF_FIRST:
	  case BUF_LAST:
	    break;
	  default:
	    /* Word anchors etc. cannot be handled.  It's okay to test
	       opr.ctx_type since constraints (for all DFA nodes) are
	       created by ORing one or more opr.ctx_type values.  */
	    return;
	  }
	break;
      case OP_PERIOD:
	has_period = true;
	break;
      case OP_BACK_REF:
      case OP_ALT:
      case END_OF_RE:
      case OP_DUP_ASTERISK:
      case OP_OPEN_SUBEXP:
      case OP_CLOSE_SUBEXP:
	break;
      case COMPLEX_BRACKET:
	return;
      case SIMPLE_BRACKET:
	/* Just double check.  */
	{
	  int rshift = (ASCII_CHARS % BITSET_WORD_BITS == 0
			? 0
			: BITSET_WORD_BITS - ASCII_CHARS % BITSET_WORD_BITS);
	  for (i = ASCII_CHARS / BITSET_WORD_BITS; i < BITSET_WORDS; ++i)
	    {
	      if (dfa->nodes[node].opr.sbcset[i] >> rshift != 0)
		return;
	      rshift = 0;
	    }
	}
	break;
      default:
	abort ();
      }

  if (mb_chars || has_period)
    for (node = 0; node < dfa->nodes_len; ++node)
      {
	if (dfa->nodes[node].type == CHARACTER
	    && dfa->nodes[node].opr.c >= ASCII_CHARS)
	  dfa->nodes[node].mb_partial = 0;
	else if (dfa->nodes[node].type == OP_PERIOD)
	  dfa->nodes[node].type = OP_UTF8_PERIOD;
      }

  /* The search can be in single byte locale.  */
  dfa->mb_cur_max = 1;
  dfa->is_utf8 = 0;
  dfa->has_mb_node = dfa->nbackref > 0 || has_period;
}