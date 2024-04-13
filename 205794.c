table_map Item_direct_view_ref::used_tables() const
{
  DBUG_ASSERT(fixed);

  if (get_depended_from())
    return OUTER_REF_TABLE_BIT;

  if (view->is_merged_derived() || view->merged || !view->table)
  {
    table_map used= (*ref)->used_tables();
    return (used ?
            used :
            ((null_ref_table != NO_NULL_TABLE) ?
             null_ref_table->map :
             (table_map)0 ));
  }
  return view->table->map;
}