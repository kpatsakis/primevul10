void rfbScaledScreenUpdate(rfbScreenInfoPtr screen, int x1, int y1, int x2, int y2)
{
    /* ok, now the task is to update each and every scaled version of the framebuffer
     * and we only have to do this for this specific changed rectangle!
     */
    rfbScreenInfoPtr ptr;
    int count=0;

    /* We don't point to cl->screen as it is the original */
    for (ptr=screen->scaledScreenNext;ptr!=NULL;ptr=ptr->scaledScreenNext)
    {
        /* Only update if it has active clients... */
        if (ptr->scaledScreenRefCount>0)
        {
          rfbScaledScreenUpdateRect(screen, ptr, x1, y1, x2-x1, y2-y1);
          count++;
        }
    }
}