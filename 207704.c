JOIN::init_range_rowid_filters()
{
  DBUG_ENTER("init_range_rowid_filters");

  JOIN_TAB *tab;

  for (tab= first_linear_tab(this, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
       tab;
       tab= next_linear_tab(this, tab, WITH_BUSH_ROOTS))
  {
    if (!tab->rowid_filter)
      continue;
    if (tab->rowid_filter->get_container()->alloc())
    {
      delete tab->rowid_filter;
      tab->rowid_filter= 0;
      continue;
    }
    tab->table->file->rowid_filter_push(tab->rowid_filter);
    tab->is_rowid_filter_built= false;
  }
  DBUG_RETURN(0);
}