Item_ident::Item_ident(TABLE_LIST *view_arg, const char *field_name_arg)
  :orig_db_name(NullS), orig_table_name(view_arg->table_name),
   orig_field_name(field_name_arg), context(&view_arg->view->select_lex.context),
   db_name(NullS), table_name(view_arg->alias),
   field_name(field_name_arg),
   alias_name_used(FALSE), cached_field_index(NO_CACHED_FIELD_INDEX),
   cached_table(NULL), depended_from(NULL), can_be_depended(TRUE)
{
  name = (char*) field_name_arg;
  name_length= name ? strlen(name) : 0;
}