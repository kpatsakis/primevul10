Field *create_tmp_field(TABLE *table, Item *item,
                        Item ***copy_func, Field **from_field,
                        Field **default_field,
                        bool group, bool modify_item,
                        bool table_cant_handle_bit_fields,
                        bool make_copy_field)
{
  Tmp_field_src src;
  Tmp_field_param prm(group, modify_item, table_cant_handle_bit_fields,
                      make_copy_field);
  Field *result= item->create_tmp_field_ex(table, &src, &prm);
  *from_field= src.field();
  *default_field= src.default_field();
  if (src.item_result_field())
    *((*copy_func)++)= src.item_result_field();
  return result;
}