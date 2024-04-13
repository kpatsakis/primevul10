token_acknowledged(unsigned long token, struct timeval *now)
{
  ResponseCell *last_valid, *cell, *next;

  last_valid = &kept_replies;
  cell = kept_replies.next;
  
  while(cell) {
    next = cell->next;

    /* Discard if it's the one or if the reply is stale */
    if ((cell->next_tok == token) ||
        ((now->tv_sec - cell->ts.tv_sec) > REPLY_MAXAGE)) {
      free_reply_slot(cell);
      last_valid->next = next;
    } else {
      last_valid->next = cell;
      last_valid = cell;
    }
    cell = next;
  }
}