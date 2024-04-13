void Item_outer_ref::fix_after_pullout(st_select_lex *new_parent, Item **ref)
{
  if (get_depended_from() == new_parent)
  {
    *ref= outer_ref;
    (*ref)->fix_after_pullout(new_parent, ref);
  }
}