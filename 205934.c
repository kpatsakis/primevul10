void Item_field::save_org_in_field(Field *to)
{
  save_field_in_field(field, &null_value, to, TRUE);
}