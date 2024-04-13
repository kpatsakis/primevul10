void rfbScalingSetup(rfbClientPtr cl, int width, int height)
{
    rfbScreenInfoPtr ptr;

    ptr = rfbScalingFind(cl,width,height);
    if (ptr==NULL)
        ptr = rfbScaledScreenAllocate(cl,width,height);
    /* Now, there is a new screen available (if ptr is not NULL) */
    if (ptr!=NULL)
    {
        /* Update it! */
        if (ptr->scaledScreenRefCount<1)
            rfbScaledScreenUpdateRect(cl->screen, ptr, 0, 0, cl->screen->width, cl->screen->height);
        /*
         * rfbLog("Taking one from %dx%d-%d and adding it to %dx%d-%d\n",
         *    cl->scaledScreen->width, cl->scaledScreen->height,
         *    cl->scaledScreen->scaledScreenRefCount,
         *    ptr->width, ptr->height, ptr->scaledScreenRefCount);
         */

        LOCK(cl->updateMutex);
        cl->scaledScreen->scaledScreenRefCount--;
        ptr->scaledScreenRefCount++;
        cl->scaledScreen=ptr;
        cl->newFBSizePending = TRUE;
        UNLOCK(cl->updateMutex);

        rfbLog("Scaling to %dx%d (refcount=%d)\n",width,height,ptr->scaledScreenRefCount);
    }
    else
        rfbLog("Scaling to %dx%d failed, leaving things alone\n",width,height);
}