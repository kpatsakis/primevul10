bool Item_ident::remove_dependence_processor(uchar * arg)
{
  DBUG_ENTER("Item_ident::remove_dependence_processor");
  if (get_depended_from() == (st_select_lex *) arg)
    depended_from= 0;
  context= &((st_select_lex *) arg)->context;
  DBUG_RETURN(0);
}