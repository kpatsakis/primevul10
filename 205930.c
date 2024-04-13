void Item_ref::cleanup()
{
  DBUG_ENTER("Item_ref::cleanup");
  Item_ident::cleanup();
  result_field= 0;
  if (reference_trough_name)
  {
    /* We have to reset the reference as it may been freed */
    ref= 0;
  }
  DBUG_VOID_RETURN;
}