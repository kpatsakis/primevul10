rfbClientIteratorHead(rfbClientIteratorPtr i)
{
#if defined(LIBVNCSERVER_HAVE_LIBPTHREAD) || defined(LIBVNCSERVER_HAVE_WIN32THREADS)
  if(i->next != 0) {
    rfbDecrClientRef(i->next);
    rfbIncrClientRef(i->screen->clientHead);
  }
#endif
  LOCK(rfbClientListMutex);
  i->next = i->screen->clientHead;
  UNLOCK(rfbClientListMutex);
  return i->next;
}