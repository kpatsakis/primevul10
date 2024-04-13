Item_ref::Item_ref(Name_resolution_context *context_arg,
                   Item **item, const char *table_name_arg,
                   const char *field_name_arg,
                   bool alias_name_used_arg)
  :Item_ident(context_arg, NullS, table_name_arg, field_name_arg),
   result_field(0), ref(item), reference_trough_name(0)
{
  alias_name_used= alias_name_used_arg;
  /*
    This constructor used to create some internals references over fixed items
  */
  if ((set_properties_only= (ref && *ref && (*ref)->fixed)))
    set_properties();
}