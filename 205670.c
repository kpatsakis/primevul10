void Item::print_value(String *str)
{
  char buff[MAX_FIELD_WIDTH];
  String *ptr, tmp(buff,sizeof(buff),str->charset());
  ptr= val_str(&tmp);
  if (!ptr)
    str->append("NULL");
  else
  {
    switch (result_type()) {
    case STRING_RESULT:
      append_unescaped(str, ptr->ptr(), ptr->length());
      break;
    case DECIMAL_RESULT:
    case REAL_RESULT:
    case INT_RESULT:
      str->append(*ptr);
      break;
    case ROW_RESULT:
    case TIME_RESULT:
    case IMPOSSIBLE_RESULT:
      DBUG_ASSERT(0);
    }
  }
}