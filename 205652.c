void Item_field::print(String *str, enum_query_type query_type)
{
  if (field && field->table->const_table &&
      !(query_type & QT_NO_DATA_EXPANSION))
  {
    print_value(str);
    return;
  }
  Item_ident::print(str, query_type);
}