get_more_replies(void)
{
  ResponseCell *new_replies;
  int i;

  if (!free_replies) {
    new_replies = MallocArray(ResponseCell, REPLY_EXTEND_QUANTUM);
    for (i=1; i<REPLY_EXTEND_QUANTUM; i++) {
      new_replies[i-1].next = new_replies + i;
    }
    free_replies = new_replies;
  }
}