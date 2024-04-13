lookup_reply(unsigned long prev_msg_token, unsigned long client_msg_seq, unsigned short attempt)
{
  ResponseCell *ptr;

  ptr = kept_replies.next;
  while (ptr) {
    if ((ptr->tok == prev_msg_token) &&
        (ptr->msg_seq == client_msg_seq) &&
        ((unsigned long) attempt > ptr->attempt)) {

      /* Set the attempt field to remember the highest number we have
         had so far */
      ptr->attempt = (unsigned long) attempt;
      return &ptr->rpy;
    }
    ptr = ptr->next;
  }

  return NULL;
}