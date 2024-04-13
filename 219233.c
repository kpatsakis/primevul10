rfbScreenInfoPtr rfbScalingFind(rfbClientPtr cl, int width, int height)
{
    rfbScreenInfoPtr ptr;
    /* include the original in the search (ie: fine 1:1 scaled version of the frameBuffer) */
    for (ptr=cl->screen; ptr!=NULL; ptr=ptr->scaledScreenNext)
    {
        if ((ptr->width==width) && (ptr->height==height))
            return ptr;
    }
    return NULL;
}