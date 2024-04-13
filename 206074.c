table_map Item_ref::used_tables() const		
{
  return get_depended_from() ? OUTER_REF_TABLE_BIT : (*ref)->used_tables(); 
}