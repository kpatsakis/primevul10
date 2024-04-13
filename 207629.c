void JOIN_TAB::build_range_rowid_filter_if_needed()
{
  if (rowid_filter && !is_rowid_filter_built)
  {
    /**
      The same handler object (table->file) is used to build a filter
      and to perfom a primary table access (by the main query).

      To estimate the time for filter building tracker should be changed
      and after building of the filter has been finished it should be
      switched back to the previos tracker.
    */
    Exec_time_tracker *table_tracker= table->file->get_time_tracker();
    Rowid_filter_tracker *rowid_tracker= rowid_filter->get_tracker();
    table->file->set_time_tracker(rowid_tracker->get_time_tracker());
    rowid_tracker->start_tracking();
    if (!rowid_filter->build())
    {
      is_rowid_filter_built= true;
    }
    else
    {
      delete rowid_filter;
      rowid_filter= 0;
    }
    rowid_tracker->stop_tracking();
    table->file->set_time_tracker(table_tracker);
  }
}