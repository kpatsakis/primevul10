void rfbDecrClientRef(rfbClientPtr cl)
{
  LOCK(cl->refCountMutex);
  cl->refCount--;
  if(cl->refCount<=0) /* just to be sure also < 0 */
    TSIGNAL(cl->deleteCond);
  UNLOCK(cl->refCountMutex);
}