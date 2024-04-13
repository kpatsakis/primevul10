bool Item_trigger_field::eq(const Item *item, bool binary_cmp) const
{
  return item->type() == TRIGGER_FIELD_ITEM &&
         row_version == ((Item_trigger_field *)item)->row_version &&
         !my_strcasecmp(system_charset_info, field_name,
                        ((Item_trigger_field *)item)->field_name);
}