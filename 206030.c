bool Item_field::vcol_in_partition_func_processor(uchar *int_arg)
{
  DBUG_ASSERT(fixed);
  if (field->vcol_info)
  {
    field->vcol_info->mark_as_in_partitioning_expr();
  }
  return FALSE;
}