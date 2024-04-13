  for (p = head; p; ) {
    next_node = p->next;
    Free(p);
    p = next_node;
  }