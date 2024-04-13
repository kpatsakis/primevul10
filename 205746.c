my_decimal *Item_cache_wrapper::val_decimal(my_decimal* decimal_value)
{
  Item *cached_value;
  DBUG_ENTER("Item_cache_wrapper::val_decimal");
  if (!expr_cache)
  {
    my_decimal *tmp= orig_item->val_decimal(decimal_value);
    null_value= orig_item->null_value;
    DBUG_RETURN(tmp);
  }

  if ((cached_value= check_cache()))
  {
    my_decimal *tmp= cached_value->val_decimal(decimal_value);
    null_value= cached_value->null_value;
    DBUG_RETURN(tmp);
  }
  cache();
  if ((null_value= expr_value->null_value))
    DBUG_RETURN(NULL);
  DBUG_RETURN(expr_value->val_decimal(decimal_value));
}