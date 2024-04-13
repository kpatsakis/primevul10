release_ts_cell(TimestampCell *node)
{
  node->next = free_ts_list;
  free_ts_list = node;
}