Item::Type Item_name_const::type() const
{
  /*
    As 
    1. one can try to create the Item_name_const passing non-constant 
    arguments, although it's incorrect and 
    2. the type() method can be called before the fix_fields() to get
    type information for a further type cast, e.g. 
    if (item->type() == FIELD_ITEM) 
      ((Item_field *) item)->... 
    we return NULL_ITEM in the case to avoid wrong casting.

    valid_args guarantees value_item->basic_const_item(); if type is
    FUNC_ITEM, then we have a fudged item_func_neg() on our hands
    and return the underlying type.
    For Item_func_set_collation()
    e.g. NAME_CONST('name', 'value' COLLATE collation) we return its
    'value' argument type. 
  */
  if (!valid_args)
    return NULL_ITEM;
  Item::Type value_type= value_item->type();
  if (value_type == FUNC_ITEM)
  {
    /* 
      The second argument of NAME_CONST('name', 'value') must be 
      a simple constant item or a NEG_FUNC/COLLATE_FUNC.
    */
    DBUG_ASSERT(((Item_func *) value_item)->functype() == 
                Item_func::NEG_FUNC ||
                ((Item_func *) value_item)->functype() == 
                Item_func::COLLATE_FUNC);
    return ((Item_func *) value_item)->key_item()->type();            
  }
  return value_type;
}