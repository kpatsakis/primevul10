static void trace_table_dependencies(THD *thd,
                                     JOIN_TAB *join_tabs, uint table_count)
{
  DBUG_ASSERT(thd->trace_started());
  Json_writer_object trace_wrapper(thd);
  Json_writer_array trace_dep(thd, "table_dependencies");

  for (uint i= 0; i < table_count; i++)
  {
    TABLE_LIST *table_ref= join_tabs[i].tab_list;
    Json_writer_object trace_one_table(thd);
    trace_one_table.add_table_name(&join_tabs[i]);
    trace_one_table.add("row_may_be_null",
                       (bool)table_ref->table->maybe_null);
    const table_map map= table_ref->get_map();
    DBUG_ASSERT(map < (1ULL << table_count));
    for (uint j= 0; j < table_count; j++)
    {
      if (map & (1ULL << j))
      {
        trace_one_table.add("map_bit", j);
        break;
      }
    }
    Json_writer_array depends_on(thd, "depends_on_map_bits");
    Table_map_iterator it(join_tabs[i].dependent);
    uint dep_bit;
    while ((dep_bit= it++) != Table_map_iterator::BITMAP_END)
       depends_on.add(static_cast<longlong>(dep_bit));
  }
}