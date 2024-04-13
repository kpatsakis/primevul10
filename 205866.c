void view_error_processor(THD *thd, void *data)
{
  ((TABLE_LIST *)data)->hide_view_error(thd);
}