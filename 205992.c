Item_ref::Item_ref(TABLE_LIST *view_arg, Item **item,
                   const char *field_name_arg, bool alias_name_used_arg)
  :Item_ident(view_arg, field_name_arg),
   result_field(NULL), ref(item), reference_trough_name(0)
{
  alias_name_used= alias_name_used_arg;
  /*
    This constructor is used to create some internal references over fixed items
  */
  if ((set_properties_only= (ref && *ref && (*ref)->fixed)))
    set_properties();
}