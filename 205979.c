void Item_param::print(String *str, enum_query_type query_type)
{
  if (state == NO_VALUE || query_type & QT_NO_DATA_EXPANSION)
  {
    str->append('?');
  }
  else
  {
    char buffer[STRING_BUFFER_USUAL_SIZE];
    String tmp(buffer, sizeof(buffer), &my_charset_bin);
    const String *res;
    res= query_val_str(current_thd, &tmp);
    str->append(*res);
  }
}