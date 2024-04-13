rfbClientIteratorHead(rfbClientIteratorPtr i)
{
  LOCK(rfbClientListMutex);
  i->next = i->screen->rfbClientHead;
  UNLOCK(rfbClientListMutex);
  return i->next;
}