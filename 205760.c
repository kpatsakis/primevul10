bool Item::fix_fields(THD *thd, Item **ref)
{

  // We do not check fields which are fixed during construction
  DBUG_ASSERT(fixed == 0 || basic_const_item());
  fixed= 1;
  return FALSE;
}