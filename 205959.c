void Item_ref_null_helper::save_val(Field *to)
{
  DBUG_ASSERT(fixed == 1);
  (*ref)->save_val(to);
  owner->was_null|= null_value= (*ref)->null_value;
}