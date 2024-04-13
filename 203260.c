free_reply_slot(ResponseCell *cell)
{
  cell->next = free_replies;
  free_replies = cell;
}