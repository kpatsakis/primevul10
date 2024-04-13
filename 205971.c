table_map Item_direct_view_ref::not_null_tables() const
{
  return get_depended_from() ?
         0 :
         ((view->is_merged_derived() || view->merged || !view->table) ?
          (*ref)->not_null_tables() :
          view->table->map);
}