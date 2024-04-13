save_reply(CMD_Reply *msg,
           unsigned long tok_reply_to,
           unsigned long new_tok_issued,
           unsigned long client_msg_seq,
           unsigned short attempt,
           struct timeval *now)
{
  ResponseCell *cell;

  cell = get_reply_slot();

  cell->ts = *now;
  memcpy(&cell->rpy, msg, sizeof(CMD_Reply));
  cell->tok = tok_reply_to;
  cell->next_tok = new_tok_issued;
  cell->msg_seq = client_msg_seq;
  cell->attempt = (unsigned long) attempt;

  cell->next = kept_replies.next;
  kept_replies.next = cell;

}