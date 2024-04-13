get_reply_slot(void)
{
  ResponseCell *result;
  if (!free_replies) {
    get_more_replies();
  }
  result = free_replies;
  free_replies = result->next;
  return result;
}