rfbScreenInfoPtr rfbScaledScreenAllocate(rfbClientPtr cl, int width, int height)
{
    rfbScreenInfoPtr ptr;
    ptr = malloc(sizeof(rfbScreenInfo));
    if (ptr!=NULL)
    {
        int allocSize;

        /* copy *everything* (we don't use most of it, but just in case) */
        memcpy(ptr, cl->screen, sizeof(rfbScreenInfo));

        /* SECURITY: make sure that no integer overflow will occur afterwards.
         * Note: this is defensive coding, as the check should have already been
         * performed during initial, non-scaled screen setup.
         */
        allocSize = pad4(width * (ptr->bitsPerPixel/8)); /* per protocol, width<2**16 and bpp<256 */
        if (height == 0 || allocSize >= SIZE_MAX / height)
        {
          free(ptr);
          return NULL; /* malloc() will allocate an incorrect buffer size - early abort */
        }

        /* Resume copy everything */
        ptr->width = width;
        ptr->height = height;
        ptr->paddedWidthInBytes = (ptr->bitsPerPixel/8)*ptr->width;

        /* Need to by multiples of 4 for Sparc systems */
        ptr->paddedWidthInBytes = pad4(ptr->paddedWidthInBytes);

        /* Reset the reference count to 0! */
        ptr->scaledScreenRefCount = 0;

        ptr->sizeInBytes = ptr->paddedWidthInBytes * ptr->height;
        ptr->serverFormat = cl->screen->serverFormat;

        ptr->frameBuffer = malloc(ptr->sizeInBytes);
        if (ptr->frameBuffer!=NULL)
        {
            /* Reset to a known condition: scale the entire framebuffer */
            rfbScaledScreenUpdateRect(cl->screen, ptr, 0, 0, cl->screen->width, cl->screen->height);
            /* Now, insert into the chain */
            LOCK(cl->updateMutex);
            ptr->scaledScreenNext = cl->screen->scaledScreenNext;
            cl->screen->scaledScreenNext = ptr;
            UNLOCK(cl->updateMutex);
        }
        else
        {
            /* Failed to malloc the new frameBuffer, cleanup */
            free(ptr);
            ptr=NULL;
        }
    }
    return ptr;
}