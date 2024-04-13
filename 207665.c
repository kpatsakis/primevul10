Field *Item_func_sp::create_tmp_field_ex(TABLE *table,
                                         Tmp_field_src *src,
                                         const Tmp_field_param *param)
{
  Field *result;
  get_tmp_field_src(src, param);
  if ((result= sp_result_field->create_tmp_field(table->in_use->mem_root,
                                                 table)))
  {
    result->field_name= name;
    if (param->modify_item())
      result_field= result;
  }
  return result;
}