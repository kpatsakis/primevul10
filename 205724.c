Item *Item_ref::get_tmp_table_item(THD *thd)
{
  if (!result_field)
    return (*ref)->get_tmp_table_item(thd);

  Item_field *item= new Item_field(result_field);
  if (item)
  {
    item->table_name= table_name;
    item->db_name= db_name;
  }
  return item;
}