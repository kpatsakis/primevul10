void Item_ref::fix_after_pullout(st_select_lex *new_parent, Item **refptr)
{
  (*ref)->fix_after_pullout(new_parent, ref);
  if (get_depended_from() == new_parent)
    depended_from= NULL;
}