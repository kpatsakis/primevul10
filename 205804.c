void Item::cleanup()
{
  DBUG_ENTER("Item::cleanup");
  DBUG_PRINT("enter", ("this: %p", this));
  fixed=0;
  marker= 0;
  join_tab_idx= MAX_TABLES;
  if (orig_name)
    name= orig_name;
  DBUG_VOID_RETURN;
}