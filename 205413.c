void eval_expr(VAR *v, const char *p, const char **p_end,
               bool open_end, bool do_eval)
{

  DBUG_ENTER("eval_expr");
  DBUG_PRINT("enter", ("p: '%s'", p));

  /* Skip to treat as pure string if no evaluation */
  if (! do_eval)
    goto NO_EVAL;
  
  if (*p == '$')
  {
    VAR *vp;
    const char* expected_end= *p_end; // Remember var end
    if ((vp= var_get(p, p_end, 0, 0)))
      var_copy(v, vp);

    /* Apparently it is not safe to assume null-terminated string */
    v->str_val[v->str_val_len]= 0;

    /* Make sure there was just a $variable and nothing else */
    const char* end= *p_end + 1;
    if (end < expected_end && !open_end)
      die("Found junk '%.*s' after $variable in expression",
          (int)(expected_end - end - 1), end);

    DBUG_VOID_RETURN;
  }

  if (*p == '`')
  {
    var_query_set(v, p, p_end);
    DBUG_VOID_RETURN;
  }

  {
    /* Check if this is a "let $var= query_get_value()" */
    const char* get_value_str= "query_get_value";
    const size_t len= strlen(get_value_str);
    if (strncmp(p, get_value_str, len)==0)
    {
      struct st_command command;
      memset(&command, 0, sizeof(command));
      command.query= (char*)p;
      command.first_word_len= len;
      command.first_argument= command.query + len;
      command.end= (char*)*p_end;
      command.abort_on_error= 1; /* avoid uninitialized variables */
      var_set_query_get_value(&command, v);
      DBUG_VOID_RETURN;
    }
  }

 NO_EVAL:
  {
    int new_val_len = (p_end && *p_end) ?
      (int) (*p_end - p) : (int) strlen(p);
    if (new_val_len + 1 >= v->alloced_len)
    {
      static int MIN_VAR_ALLOC= 32;
      v->alloced_len = (new_val_len < MIN_VAR_ALLOC - 1) ?
        MIN_VAR_ALLOC : new_val_len + 1;
      if (!(v->str_val =
            v->str_val ?
            (char*)my_realloc(v->str_val, v->alloced_len+1, MYF(MY_WME)) :
            (char*)my_malloc(v->alloced_len+1, MYF(MY_WME))))
        die("Out of memory");
    }
    v->str_val_len = new_val_len;
    memcpy(v->str_val, p, new_val_len);
    v->str_val[new_val_len] = 0;
    var_check_int(v);
  }
  DBUG_VOID_RETURN;
}