bool Item_field::find_item_in_field_list_processor(uchar *arg)
{
  KEY_PART_INFO *first_non_group_part= *((KEY_PART_INFO **) arg);
  KEY_PART_INFO *last_part= *(((KEY_PART_INFO **) arg) + 1);
  KEY_PART_INFO *cur_part;

  for (cur_part= first_non_group_part; cur_part != last_part; cur_part++)
  {
    if (field->eq(cur_part->field))
      return TRUE;
  }
  return FALSE;
}