Item *Item_field::replace_equal_field(uchar *arg)
{
  REPLACE_EQUAL_FIELD_ARG* param= (REPLACE_EQUAL_FIELD_ARG*)arg;
  if (item_equal && item_equal == param->item_equal)
  {
    Item *const_item= item_equal->get_const();
    if (const_item)
    {
      if (!has_compatible_context(const_item))
        return this;
      return const_item;
    }
    Item_field *subst= 
      (Item_field *)(item_equal->get_first(param->context_tab, this));
    if (subst)
      subst= (Item_field *) (subst->real_item());
    if (subst && !field->eq(subst->field))
      return subst;
  }
  return this;
}