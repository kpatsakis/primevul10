rfbClientIteratorNext(rfbClientIteratorPtr i)
{
  if(i->next == 0) {
    LOCK(rfbClientListMutex);
    i->next = i->screen->rfbClientHead;
    UNLOCK(rfbClientListMutex);
  } else {
    i->next = i->next->next;
  }

    return i->next;
}