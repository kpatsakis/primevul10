table_map Item_field::used_tables() const
{
  if (field->table->const_table)
    return 0;					// const item
  return (get_depended_from() ? OUTER_REF_TABLE_BIT : field->table->map);
}