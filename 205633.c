Item *Item_field::equal_fields_propagator(uchar *arg)
{
  if (no_const_subst)
    return this;
  item_equal= find_item_equal((COND_EQUAL *) arg);
  Item *item= 0;
  if (item_equal)
    item= item_equal->get_const();
  /*
    Disable const propagation for items used in different comparison contexts.
    This must be done because, for example, Item_hex_string->val_int() is not
    the same as (Item_hex_string->val_str() in BINARY column)->val_int().
    We cannot simply disable the replacement in a particular context (
    e.g. <bin_col> = <int_col> AND <bin_col> = <hex_string>) since
    Items don't know the context they are in and there are functions like 
    IF (<hex_string>, 'yes', 'no').
  */
  if (!item || !has_compatible_context(item))
    item= this;
  else if (field && (field->flags & ZEROFILL_FLAG) && IS_NUM(field->type()))
  {
    if (item && (cmp_context == STRING_RESULT || cmp_context == IMPOSSIBLE_RESULT))
      convert_zerofill_number_to_string(&item, (Field_num *)field);
    else
      item= this;
  }
  return item;
}