Item_param::set_param_type_and_swap_value(Item_param *src)
{
  unsigned_flag= src->unsigned_flag;
  param_type= src->param_type;
  set_param_func= src->set_param_func;
  item_type= src->item_type;
  item_result_type= src->item_result_type;

  collation.set(src->collation);
  maybe_null= src->maybe_null;
  null_value= src->null_value;
  max_length= src->max_length;
  decimals= src->decimals;
  state= src->state;
  value= src->value;

  decimal_value.swap(src->decimal_value);
  str_value.swap(src->str_value);
  str_value_ptr.swap(src->str_value_ptr);
}