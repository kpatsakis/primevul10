void Item_ref::print(String *str, enum_query_type query_type)
{
  if (ref)
  {
    if ((*ref)->type() != Item::CACHE_ITEM && ref_type() != VIEW_REF &&
        !table_name && name && alias_name_used)
    {
      THD *thd= current_thd;
      append_identifier(thd, str, (*ref)->real_item()->name,
                        strlen((*ref)->real_item()->name));
    }
    else
      (*ref)->print(str, query_type);
  }
  else
    Item_ident::print(str, query_type);
}