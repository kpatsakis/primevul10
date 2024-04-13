bool Item_param::set_from_user_var(THD *thd, const user_var_entry *entry)
{
  DBUG_ENTER("Item_param::set_from_user_var");
  if (entry && entry->value)
  {
    item_result_type= entry->type;
    unsigned_flag= entry->unsigned_flag;
    if (limit_clause_param)
    {
      bool unused;
      set_int(entry->val_int(&unused), MY_INT64_NUM_DECIMAL_DIGITS);
      item_type= Item::INT_ITEM;
      DBUG_RETURN(!unsigned_flag && value.integer < 0 ? 1 : 0);
    }
    switch (item_result_type) {
    case REAL_RESULT:
      set_double(*(double*)entry->value);
      item_type= Item::REAL_ITEM;
      param_type= MYSQL_TYPE_DOUBLE;
      break;
    case INT_RESULT:
      set_int(*(longlong*)entry->value, MY_INT64_NUM_DECIMAL_DIGITS);
      item_type= Item::INT_ITEM;
      param_type= MYSQL_TYPE_LONGLONG;
      break;
    case STRING_RESULT:
    {
      CHARSET_INFO *fromcs= entry->collation.collation;
      CHARSET_INFO *tocs= thd->variables.collation_connection;
      uint32 dummy_offset;

      value.cs_info.character_set_of_placeholder= fromcs;
      value.cs_info.character_set_client= thd->variables.character_set_client;
      /*
        Setup source and destination character sets so that they
        are different only if conversion is necessary: this will
        make later checks easier.
      */
      value.cs_info.final_character_set_of_str_value=
        String::needs_conversion(0, fromcs, tocs, &dummy_offset) ?
        tocs : fromcs;
      /*
        Exact value of max_length is not known unless data is converted to
        charset of connection, so we have to set it later.
      */
      item_type= Item::STRING_ITEM;
      param_type= MYSQL_TYPE_VARCHAR;

      if (set_str((const char *)entry->value, entry->length))
        DBUG_RETURN(1);
      break;
    }
    case DECIMAL_RESULT:
    {
      const my_decimal *ent_value= (const my_decimal *)entry->value;
      my_decimal2decimal(ent_value, &decimal_value);
      state= DECIMAL_VALUE;
      decimals= ent_value->frac;
      max_length=
        my_decimal_precision_to_length_no_truncation(ent_value->precision(),
                                                     decimals, unsigned_flag);
      item_type= Item::DECIMAL_ITEM;
      param_type= MYSQL_TYPE_NEWDECIMAL;
      break;
    }
    case ROW_RESULT:
    case TIME_RESULT:
    case IMPOSSIBLE_RESULT:
      DBUG_ASSERT(0);
      set_null();
    }
  }
  else
    set_null();

  DBUG_RETURN(0);
}