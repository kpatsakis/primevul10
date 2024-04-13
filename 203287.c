allocate_ts_cell(void)
{
  TimestampCell *result;
  int i;
  if (free_ts_list == NULL) {
    free_ts_list = MallocArray(TimestampCell, EXTEND_QUANTUM);
    for (i=0; i<EXTEND_QUANTUM-1; i++) {
      free_ts_list[i].next = free_ts_list + i + 1;
    }
    free_ts_list[EXTEND_QUANTUM - 1].next = NULL;
  }

  result = free_ts_list;
  free_ts_list = free_ts_list->next;
  return result;
}