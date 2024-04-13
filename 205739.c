st_select_lex *Item_ident::get_depended_from() const
{
  st_select_lex *dep;
  if ((dep= depended_from))
    for ( ; dep->merged_into; dep= dep->merged_into) ;
  return dep;
}