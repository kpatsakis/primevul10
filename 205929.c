Item_ident::Item_ident(Name_resolution_context *context_arg,
                       const char *db_name_arg,const char *table_name_arg,
		       const char *field_name_arg)
  :orig_db_name(db_name_arg), orig_table_name(table_name_arg),
   orig_field_name(field_name_arg), context(context_arg),
   db_name(db_name_arg), table_name(table_name_arg),
   field_name(field_name_arg),
   alias_name_used(FALSE), cached_field_index(NO_CACHED_FIELD_INDEX),
   cached_table(0), depended_from(0), can_be_depended(TRUE)
{
  name = (char*) field_name_arg;
  name_length= name ? strlen(name) : 0;
}