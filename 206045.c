bool Item_field::eq(const Item *item, bool binary_cmp) const
{
  Item *real_item= ((Item *) item)->real_item();
  if (real_item->type() != FIELD_ITEM)
    return 0;
  
  Item_field *item_field= (Item_field*) real_item;
  if (item_field->field && field)
    return item_field->field == field;
  /*
    We may come here when we are trying to find a function in a GROUP BY
    clause from the select list.
    In this case the '100 % correct' way to do this would be to first
    run fix_fields() on the GROUP BY item and then retry this function, but
    I think it's better to relax the checking a bit as we will in
    most cases do the correct thing by just checking the field name.
    (In cases where we would choose wrong we would have to generate a
    ER_NON_UNIQ_ERROR).
  */
  return (!my_strcasecmp(system_charset_info, item_field->name,
			 field_name) &&
	  (!item_field->table_name || !table_name ||
	   (!my_strcasecmp(table_alias_charset, item_field->table_name,
			   table_name) &&
	    (!item_field->db_name || !db_name ||
	     (item_field->db_name && !strcmp(item_field->db_name,
					     db_name))))));
}