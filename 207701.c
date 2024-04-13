Field *Item_ref::create_tmp_field_ex(TABLE *table,
                                     Tmp_field_src *src,
                                     const Tmp_field_param *param)
{
  Item *item= real_item();
  DBUG_ASSERT(is_result_field());
  if (item->type() == Item::FIELD_ITEM)
  {
    Field *result;
    Item_field *field= (Item_field*) item;
    Tmp_field_param prm2(*param);
    prm2.set_modify_item(false);
    src->set_field(field->field);
    if (!(result= field->create_tmp_field_from_item_field(table, this, &prm2)))
      return NULL;
    if (param->modify_item())
      result_field= result;
    return result;
  }
  return Item_result_field::create_tmp_field_ex(table, src, param);
}