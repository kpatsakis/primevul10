void Item_type_holder::get_full_info(Item *item)
{
  if (fld_type == MYSQL_TYPE_ENUM ||
      fld_type == MYSQL_TYPE_SET)
  {
    if (item->type() == Item::SUM_FUNC_ITEM &&
        (((Item_sum*)item)->sum_func() == Item_sum::MAX_FUNC ||
         ((Item_sum*)item)->sum_func() == Item_sum::MIN_FUNC))
      item = ((Item_sum*)item)->get_arg(0);
    /*
      We can have enum/set type after merging only if we have one enum|set
      field (or MIN|MAX(enum|set field)) and number of NULL fields
    */
    DBUG_ASSERT((enum_set_typelib &&
                 get_real_type(item) == MYSQL_TYPE_NULL) ||
                (!enum_set_typelib &&
                 item->real_item()->type() == Item::FIELD_ITEM &&
                 (get_real_type(item->real_item()) == MYSQL_TYPE_ENUM ||
                  get_real_type(item->real_item()) == MYSQL_TYPE_SET) &&
                 ((Field_enum*)((Item_field *) item->real_item())->field)->typelib));
    if (!enum_set_typelib)
    {
      enum_set_typelib= ((Field_enum*)((Item_field *) item->real_item())->field)->typelib;
    }
  }
}