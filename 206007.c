bool Item_direct_view_ref::subst_argument_checker(uchar **arg)
{
  bool res= FALSE;
  if (*arg)
  { 
    Item *item= real_item();
    if (item->type() == FIELD_ITEM &&
        (*arg == (uchar *) Item::ANY_SUBST || 
         result_type() != STRING_RESULT ||
         (((Item_field *) item)->field->flags & BINARY_FLAG)))
      res= TRUE;
  }
  /* Block any substitution into the wrapped object */
  if (*arg)
    *arg= NULL; 
  return res; 
}