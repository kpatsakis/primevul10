void do_eval(DYNAMIC_STRING *query_eval, const char *query,
             const char *query_end, my_bool pass_through_escape_chars)
{
  const char *p;
  register char c, next_c;
  register int escaped = 0;
  VAR *v;
  DBUG_ENTER("do_eval");

  for (p= query; (c= *p) && p < query_end; ++p)
  {
    switch(c) {
    case '$':
      if (escaped)
      {
	escaped= 0;
	dynstr_append_mem(query_eval, p, 1);
      }
      else
      {
	if (!(v= var_get(p, &p, 0, 0)))
        {
          report_or_die( "Bad variable in eval");
          return;
        }
	dynstr_append_mem(query_eval, v->str_val, v->str_val_len);
      }
      break;
    case '\\':
      next_c= *(p+1);
      if (escaped)
      {
	escaped= 0;
	dynstr_append_mem(query_eval, p, 1);
      }
      else if (next_c == '\\' || next_c == '$' || next_c == '"')
      {
        /* Set escaped only if next char is \, " or $ */
	escaped= 1;

        if (pass_through_escape_chars)
        {
          /* The escape char should be added to the output string. */
          dynstr_append_mem(query_eval, p, 1);
        }
      }
      else
	dynstr_append_mem(query_eval, p, 1);
      break;
    default:
      escaped= 0;
      dynstr_append_mem(query_eval, p, 1);
      break;
    }
  }
#ifdef __WIN__
    fix_win_paths(query_eval->str, query_eval->length);
#endif
  DBUG_VOID_RETURN;
}