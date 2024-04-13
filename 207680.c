Field *Item_result_field::create_tmp_field_ex(TABLE *table,
                                              Tmp_field_src *src,
                                              const Tmp_field_param *param)
{
  /*
    Possible Item types:
    - Item_cache_wrapper  (only for CREATE..SELECT ?)
    - Item_func
    - Item_subselect
  */
  DBUG_ASSERT(is_result_field());
  DBUG_ASSERT(type() != NULL_ITEM);
  get_tmp_field_src(src, param);
  Field *result;
  if ((result= tmp_table_field_from_field_type(table)) && param->modify_item())
    result_field= result;
  return result;
}