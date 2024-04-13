table_map Item_field::all_used_tables() const
{
  return (get_depended_from() ? OUTER_REF_TABLE_BIT : field->table->map);
}