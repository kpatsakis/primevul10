Item *Item_field::update_value_transformer(uchar *select_arg)
{
  SELECT_LEX *select= (SELECT_LEX*)select_arg;
  DBUG_ASSERT(fixed);

  if (field->table != select->context.table_list->table &&
      type() != Item::TRIGGER_FIELD_ITEM)
  {
    List<Item> *all_fields= &select->join->all_fields;
    Item **ref_pointer_array= select->ref_pointer_array;
    DBUG_ASSERT(all_fields->elements <= select->ref_pointer_array_size);
    int el= all_fields->elements;
    Item_ref *ref;

    ref_pointer_array[el]= (Item*)this;
    all_fields->push_front((Item*)this);
    ref= new Item_ref(&select->context, ref_pointer_array + el,
                      table_name, field_name);
    return ref;
  }
  return this;
}