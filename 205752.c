void Item::split_sum_func2(THD *thd, Item **ref_pointer_array,
                           List<Item> &fields, Item **ref, 
                           bool skip_registered)
{
  /* An item of type Item_sum  is registered <=> ref_by != 0 */ 
  if (type() == SUM_FUNC_ITEM && skip_registered && 
      ((Item_sum *) this)->ref_by)
    return;
  if ((type() != SUM_FUNC_ITEM && with_sum_func) ||
      (type() == FUNC_ITEM &&
       (((Item_func *) this)->functype() == Item_func::ISNOTNULLTEST_FUNC ||
        ((Item_func *) this)->functype() == Item_func::TRIG_COND_FUNC)))
  {
    /* Will split complicated items and ignore simple ones */
    split_sum_func(thd, ref_pointer_array, fields);
  }
  else if ((type() == SUM_FUNC_ITEM || (used_tables() & ~PARAM_TABLE_BIT)) &&
           type() != SUBSELECT_ITEM &&
           (type() != REF_ITEM ||
           ((Item_ref*)this)->ref_type() == Item_ref::VIEW_REF))
  {
    /*
      Replace item with a reference so that we can easily calculate
      it (in case of sum functions) or copy it (in case of fields)

      The test above is to ensure we don't do a reference for things
      that are constants (PARAM_TABLE_BIT is in effect a constant)
      or already referenced (for example an item in HAVING)
      Exception is Item_direct_view_ref which we need to convert to
      Item_ref to allow fields from view being stored in tmp table.
    */
    Item_aggregate_ref *item_ref;
    uint el= fields.elements;
    DBUG_ASSERT(fields.elements <=
                thd->lex->current_select->ref_pointer_array_size);
    /*
      If this is an item_ref, get the original item
      This is a safety measure if this is called for things that is
      already a reference.
    */
    Item *real_itm= real_item();

    ref_pointer_array[el]= real_itm;
    if (!(item_ref= new Item_aggregate_ref(&thd->lex->current_select->context,
                                           ref_pointer_array + el, 0, name)))
      return;                                   // fatal_error is set
    if (type() == SUM_FUNC_ITEM)
      item_ref->depended_from= ((Item_sum *) this)->depended_from(); 
    fields.push_front(real_itm);
    thd->change_item_tree(ref, item_ref);
  }
}