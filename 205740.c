Item_name_const::Item_name_const(Item *name_arg, Item *val):
    value_item(val), name_item(name_arg)
{
  Item::maybe_null= TRUE;
  valid_args= true;
  if (!name_item->basic_const_item())
    goto err;

  if (value_item->basic_const_item())
    return; // ok

  if (value_item->type() == FUNC_ITEM)
  {
    Item_func *value_func= (Item_func *) value_item;
    if (value_func->functype() != Item_func::COLLATE_FUNC &&
        value_func->functype() != Item_func::NEG_FUNC)
      goto err;

    if (value_func->key_item()->basic_const_item())
      return; // ok
  }

err:
  valid_args= false;
  my_error(ER_WRONG_ARGUMENTS, MYF(0), "NAME_CONST");
}