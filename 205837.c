void Item_ident::print(String *str, enum_query_type query_type)
{
  THD *thd= current_thd;
  char d_name_buff[MAX_ALIAS_NAME], t_name_buff[MAX_ALIAS_NAME];
  const char *d_name= db_name, *t_name= table_name;
  if (lower_case_table_names== 1 ||
      (lower_case_table_names == 2 && !alias_name_used))
  {
    if (table_name && table_name[0])
    {
      strmov(t_name_buff, table_name);
      my_casedn_str(files_charset_info, t_name_buff);
      t_name= t_name_buff;
    }
    if (db_name && db_name[0])
    {
      strmov(d_name_buff, db_name);
      my_casedn_str(files_charset_info, d_name_buff);
      d_name= d_name_buff;
    }
  }

  if (!table_name || !field_name || !field_name[0])
  {
    const char *nm= (field_name && field_name[0]) ?
                      field_name : name ? name : "tmp_field";
    append_identifier(thd, str, nm, (uint) strlen(nm));
    return;
  }
  if (db_name && db_name[0] && !alias_name_used)
  {
    if (!(cached_table && cached_table->belong_to_view &&
          cached_table->belong_to_view->compact_view_format))
    {
      append_identifier(thd, str, d_name, (uint)strlen(d_name));
      str->append('.');
    }
    append_identifier(thd, str, t_name, (uint)strlen(t_name));
    str->append('.');
    append_identifier(thd, str, field_name, (uint)strlen(field_name));
  }
  else
  {
    if (table_name[0])
    {
      append_identifier(thd, str, t_name, (uint) strlen(t_name));
      str->append('.');
      append_identifier(thd, str, field_name, (uint) strlen(field_name));
    }
    else
      append_identifier(thd, str, field_name, (uint) strlen(field_name));
  }
}