void Item_cache_wrapper::cleanup()
{
  DBUG_ENTER("Item_cache_wrapper::cleanup");
  Item_result_field::cleanup();
  delete expr_cache;
  expr_cache= 0;
  /* expr_value is Item so it will be destroyed from list of Items */
  expr_value= 0;
  parameters.empty();
  DBUG_VOID_RETURN;
}