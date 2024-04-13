void Item_result_field::get_tmp_field_src(Tmp_field_src *src,
                                          const Tmp_field_param *param)
{
  if (param->make_copy_field())
  {
    DBUG_ASSERT(result_field);
    src->set_field(result_field);
  }
  else
  {
    src->set_item_result_field(this); // Save for copy_funcs
  }
}