rfbGetClientIterator(rfbScreenInfoPtr rfbScreen)
{
  rfbClientIteratorPtr i =
    (rfbClientIteratorPtr)malloc(sizeof(struct rfbClientIterator));
  i->next = NULL;
  i->screen = rfbScreen;
  i->closedToo = FALSE;
  return i;
}