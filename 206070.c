void Item_field::cleanup()
{
  DBUG_ENTER("Item_field::cleanup");
  Item_ident::cleanup();
  depended_from= NULL;
  /*
    Even if this object was created by direct link to field in setup_wild()
    it will be linked correctly next time by name of field and table alias.
    I.e. we can drop 'field'.
   */
  field= result_field= 0;
  item_equal= NULL;
  null_value= FALSE;
  DBUG_VOID_RETURN;
}