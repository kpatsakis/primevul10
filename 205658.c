bool Item_direct_view_ref::fix_fields(THD *thd, Item **reference)
{
  /* view fild reference must be defined */
  DBUG_ASSERT(*ref);
  /* (*ref)->check_cols() will be made in Item_direct_ref::fix_fields */
  if ((*ref)->fixed)
  {
    Item *ref_item= (*ref)->real_item();
    if (ref_item->type() == Item::FIELD_ITEM)
    {
      /*
        In some cases we need to update table read set(see bug#47150).
        If ref item is FIELD_ITEM and fixed then field and table
        have proper values. So we can use them for update.
      */
      Field *fld= ((Item_field*) ref_item)->field;
      DBUG_ASSERT(fld && fld->table);
      if (thd->mark_used_columns == MARK_COLUMNS_READ)
        bitmap_set_bit(fld->table->read_set, fld->field_index);
    }
  }
  else if (!(*ref)->fixed &&
           ((*ref)->fix_fields(thd, ref)))
    return TRUE;

  if (Item_direct_ref::fix_fields(thd, reference))
    return TRUE;
  if (view->table && view->table->maybe_null)
    maybe_null= TRUE;
  set_null_ref_table();
  return FALSE;
}