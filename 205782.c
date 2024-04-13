Item_num *Item_uint::neg()
{
  Item_decimal *item= new Item_decimal(value, 1);
  return item->neg();
}