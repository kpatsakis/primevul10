bool agg_item_set_converter(DTCollation &coll, const char *fname,
                            Item **args, uint nargs, uint flags, int item_sep)
{
  Item **arg, *safe_args[2]= {NULL, NULL};

  /*
    For better error reporting: save the first and the second argument.
    We need this only if the the number of args is 3 or 2:
    - for a longer argument list, "Illegal mix of collations"
      doesn't display each argument's characteristics.
    - if nargs is 1, then this error cannot happen.
  */
  if (nargs >=2 && nargs <= 3)
  {
    safe_args[0]= args[0];
    safe_args[1]= args[item_sep];
  }

  THD *thd= current_thd;
  bool res= FALSE;
  uint i;

  /*
    In case we're in statement prepare, create conversion item
    in its memory: it will be reused on each execute.
  */
  Query_arena backup;
  Query_arena *arena= thd->stmt_arena->is_stmt_prepare() ?
                      thd->activate_stmt_arena_if_needed(&backup) :
                      NULL;

  for (i= 0, arg= args; i < nargs; i++, arg+= item_sep)
  {
    Item* conv;
    uint32 dummy_offset;
    if (!String::needs_conversion(1, (*arg)->collation.collation,
                                  coll.collation,
                                  &dummy_offset))
      continue;

    /*
      No needs to add converter if an "arg" is NUMERIC or DATETIME
      value (which is pure ASCII) and at the same time target DTCollation
      is ASCII-compatible. For example, no needs to rewrite:
        SELECT * FROM t1 WHERE datetime_field = '2010-01-01';
      to
        SELECT * FROM t1 WHERE CONVERT(datetime_field USING cs) = '2010-01-01';
      
      TODO: avoid conversion of any values with
      repertoire ASCII and 7bit-ASCII-compatible,
      not only numeric/datetime origin.
    */
    if ((*arg)->collation.derivation == DERIVATION_NUMERIC &&
        (*arg)->collation.repertoire == MY_REPERTOIRE_ASCII &&
        !((*arg)->collation.collation->state & MY_CS_NONASCII) &&
        !(coll.collation->state & MY_CS_NONASCII))
      continue;

    if (!(conv= (*arg)->safe_charset_converter(coll.collation)) &&
        ((*arg)->collation.repertoire == MY_REPERTOIRE_ASCII))
      conv= new Item_func_conv_charset(*arg, coll.collation, 1);

    if (!conv)
    {
      if (nargs >=2 && nargs <= 3)
      {
        /* restore the original arguments for better error message */
        args[0]= safe_args[0];
        args[item_sep]= safe_args[1];
      }
      my_coll_agg_error(args, nargs, fname, item_sep);
      res= TRUE;
      break; // we cannot return here, we need to restore "arena".
    }
    if ((*arg)->type() == Item::FIELD_ITEM)
      ((Item_field *)(*arg))->no_const_subst= 1;
    /*
      If in statement prepare, then we create a converter for two
      constant items, do it once and then reuse it.
      If we're in execution of a prepared statement, arena is NULL,
      and the conv was created in runtime memory. This can be
      the case only if the argument is a parameter marker ('?'),
      because for all true constants the charset converter has already
      been created in prepare. In this case register the change for
      rollback.
    */
    if (thd->stmt_arena->is_stmt_prepare())
      *arg= conv;
    else
      thd->change_item_tree(arg, conv);

    if (conv->fix_fields(thd, arg))
    {
      res= TRUE;
      break; // we cannot return here, we need to restore "arena".
    }
  }
  if (arena)
    thd->restore_active_arena(arena, &backup);
  return res;
}