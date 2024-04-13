fetch_number (re_string_t *input, re_token_t *token, reg_syntax_t syntax)
{
  Idx num = REG_MISSING;
  unsigned char c;
  while (1)
    {
      fetch_token (token, input, syntax);
      c = token->opr.c;
      if (BE (token->type == END_OF_RE, 0))
	return REG_ERROR;
      if (token->type == OP_CLOSE_DUP_NUM || c == ',')
	break;
      num = ((token->type != CHARACTER || c < '0' || '9' < c
	      || num == REG_ERROR)
	     ? REG_ERROR
	     : num == REG_MISSING
	     ? c - '0'
	     : MIN (RE_DUP_MAX + 1, num * 10 + c - '0'));
    }
  return num;
}