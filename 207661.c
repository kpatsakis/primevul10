Field *Item_field::create_tmp_field_ex(TABLE *table,
                                       Tmp_field_src *src,
                                       const Tmp_field_param *param)
{
  DBUG_ASSERT(!is_result_field());
  Field *result;
  src->set_field(field);
  if (!(result= create_tmp_field_from_item_field(table, NULL, param)))
    return NULL;
  if (field->eq_def(result))
    src->set_default_field(field);
  return result;
}