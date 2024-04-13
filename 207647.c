Field *Item::tmp_table_field_from_field_type_maybe_null(TABLE *table,
                                            Tmp_field_src *src,
                                            const Tmp_field_param *param,
                                            bool is_explicit_null)
{
  /*
    item->type() == CONST_ITEM excluded due to making fields for counter
    With help of Item_uint
  */
  DBUG_ASSERT(!param->make_copy_field() || type() == CONST_ITEM);
  DBUG_ASSERT(!is_result_field());
  Field *result;
  if ((result= tmp_table_field_from_field_type(table)))
  {
    if (result && is_explicit_null)
      result->is_created_from_null_item= true;
  }
  return result;
}