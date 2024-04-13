Field *Item_func_user_var::create_tmp_field_ex(TABLE *table,
                                               Tmp_field_src *src,
                                               const Tmp_field_param *param)
{
  DBUG_ASSERT(is_result_field());
  DBUG_ASSERT(type() != NULL_ITEM);
  get_tmp_field_src(src, param);
  Field *result;
  if ((result= create_table_field_from_handler(table)) && param->modify_item())
    result_field= result;
  return result;
}