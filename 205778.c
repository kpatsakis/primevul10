my_decimal *Item_type_holder::val_decimal(my_decimal *)
{
  DBUG_ASSERT(0); // should never be called
  return 0;
}