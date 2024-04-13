bool JOIN::make_range_rowid_filters()
{
  DBUG_ENTER("make_range_rowid_filters");

  /*
    Do not build range filters with detected impossible WHERE.
    Anyway conditions cannot be used anymore to extract ranges for filters.
  */
  if (const_table_map != found_const_table_map)
    DBUG_RETURN(0);

  JOIN_TAB *tab;

  for (tab= first_linear_tab(this, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab;
       tab= next_linear_tab(this, tab, WITH_BUSH_ROOTS))
  {
    if (!tab->range_rowid_filter_info)
      continue;

    DBUG_ASSERT(!(tab->ref.key >= 0 &&
                  tab->ref.key == (int) tab->range_rowid_filter_info->key_no));
    DBUG_ASSERT(!(tab->ref.key == -1 && tab->quick &&
                  tab->quick->index == tab->range_rowid_filter_info->key_no));

    int err;
    SQL_SELECT *sel= NULL;
    Rowid_filter_container *filter_container= NULL;
    Item **sargable_cond= get_sargable_cond(this, tab->table);
    sel= make_select(tab->table, const_table_map, const_table_map,
                     *sargable_cond, (SORT_INFO*) 0, 1, &err);
    if (!sel)
      continue;

    key_map filter_map;
    filter_map.clear_all();
    filter_map.set_bit(tab->range_rowid_filter_info->key_no);
    filter_map.merge(tab->table->with_impossible_ranges);
    bool force_index_save= tab->table->force_index;
    tab->table->force_index= true;
    int rc= sel->test_quick_select(thd, filter_map, (table_map) 0,
                                   (ha_rows) HA_POS_ERROR,
                                   true, false, true, true);
    tab->table->force_index= force_index_save;
    if (thd->is_error())
      goto no_filter;
    /*
      If SUBS_IN_TO_EXISTS strtrategy is chosen for the subquery then
      additional conditions are injected into WHERE/ON/HAVING and it may
      happen that the call of test_quick_select() discovers impossible range.
    */
    if (rc == -1)
    {
      const_table_map|= tab->table->map;
      goto no_filter;
    }
    DBUG_ASSERT(sel->quick);
    filter_container=
      tab->range_rowid_filter_info->create_container();
    if (filter_container)
    {
      tab->rowid_filter=
        new (thd->mem_root) Range_rowid_filter(tab->table,
                                               tab->range_rowid_filter_info,
                                               filter_container, sel);
      if (tab->rowid_filter)
        continue;
    }
  no_filter:
    if (sel->quick)
      delete sel->quick;
    delete sel;
  }

  DBUG_RETURN(0);
}