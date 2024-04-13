void Item_ref::update_used_tables()
{
  if (!get_depended_from())
    (*ref)->update_used_tables();
}