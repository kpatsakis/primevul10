void Item::set_name_for_rollback(THD *thd, const char *str, uint length,
                                 CHARSET_INFO *cs)
{
  char *old_name, *new_name; 
  old_name= name;
  set_name(str, length, cs);
  new_name= name;
  if (old_name != new_name)
  {
    name= old_name;
    thd->change_item_tree((Item **) &name, (Item *) new_name);
  }
}