enum_field_types Item_type_holder::get_real_type(Item *item)
{
  if (item->type() == REF_ITEM)
    item= item->real_item();
  switch(item->type())
  {
  case FIELD_ITEM:
  {
    /*
      Item_field::field_type ask Field_type() but sometimes field return
      a different type, like for enum/set, so we need to ask real type.
    */
    Field *field= ((Item_field *) item)->field;
    enum_field_types type= field->real_type();
    if (field->is_created_from_null_item)
      return MYSQL_TYPE_NULL;
    /* work around about varchar type field detection */
    if (type == MYSQL_TYPE_STRING && field->type() == MYSQL_TYPE_VAR_STRING)
      return MYSQL_TYPE_VAR_STRING;
    return type;
  }
  case SUM_FUNC_ITEM:
  {
    /*
      Argument of aggregate function sometimes should be asked about field
      type
    */
    Item_sum *item_sum= (Item_sum *) item;
    if (item_sum->keep_field_type())
      return get_real_type(item_sum->get_arg(0));
    break;
  }
  case FUNC_ITEM:
    if (((Item_func *) item)->functype() == Item_func::GUSERVAR_FUNC)
    {
      /*
        There are work around of problem with changing variable type on the
        fly and variable always report "string" as field type to get
        acceptable information for client in send_field, so we make field
        type from expression type.
      */
      switch (item->result_type()) {
      case STRING_RESULT:
        return MYSQL_TYPE_VAR_STRING;
      case INT_RESULT:
        return MYSQL_TYPE_LONGLONG;
      case REAL_RESULT:
        return MYSQL_TYPE_DOUBLE;
      case DECIMAL_RESULT:
        return MYSQL_TYPE_NEWDECIMAL;
      case ROW_RESULT:
      case TIME_RESULT:
      case IMPOSSIBLE_RESULT:
        DBUG_ASSERT(0);
        return MYSQL_TYPE_VAR_STRING;
      }
    }
    break;
  default:
    break;
  }
  return item->field_type();
}