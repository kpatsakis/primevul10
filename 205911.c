void Item_trigger_field::cleanup()
{
  want_privilege= original_privilege;
  /*
    Since special nature of Item_trigger_field we should not do most of
    things from Item_field::cleanup() or Item_ident::cleanup() here.
  */
  Item::cleanup();
}