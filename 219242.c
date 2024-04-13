void rfbScaledCorrection(rfbScreenInfoPtr from, rfbScreenInfoPtr to, int *x, int *y, int *w, int *h, const char *function)
{
    double x1,y1,w1,h1, x2, y2, w2, h2;
    double scaleW = ((double) to->width) / ((double) from->width);
    double scaleH = ((double) to->height) / ((double) from->height);


    /*
     * rfbLog("rfbScaledCorrection(%p -> %p, %dx%d->%dx%d (%dXx%dY-%dWx%dH)\n",
     * from, to, from->width, from->height, to->width, to->height, *x, *y, *w, *h);
     */

    /* If it's the original framebuffer... */
    if (from==to) return;

    x1 = ((double) *x) * scaleW;
    y1 = ((double) *y) * scaleH;
    w1 = ((double) *w) * scaleW;
    h1 = ((double) *h) * scaleH;


    /*cast from double to int is same as "*x = floor(x1);" */
    x2 = FLOOR(x1);
    y2 = FLOOR(y1);

    /* include into W and H the jitter of scaling X and Y */
    w2 = CEIL(w1 + ( x1 - x2 ));
    h2 = CEIL(h1 + ( y1 - y2 ));

    /*
     * rfbLog("%s (%dXx%dY-%dWx%dH  ->  %fXx%fY-%fWx%fH) {%dWx%dH -> %dWx%dH}\n",
     *    function, *x, *y, *w, *h, x2, y2, w2, h2,
     *    from->width, from->height, to->width, to->height);
     */

    /* simulate ceil() without math library */
    *x = (int)x2;
    *y = (int)y2;
    *w = (int)w2;
    *h = (int)h2;

    /* Small changes for a thumbnail may be scaled to zero */
    if (*w==0) (*w)++;
    if (*h==0) (*h)++;
    /* scaling from small to big may overstep the size a bit */
    if (*x+*w > to->width)  *w=to->width - *x;
    if (*y+*h > to->height) *h=to->height - *y;
}