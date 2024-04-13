table_map Item_ref_null_helper::used_tables() const
{
  return (get_depended_from() ?
          OUTER_REF_TABLE_BIT :
          (*ref)->used_tables() | RAND_TABLE_BIT);
}