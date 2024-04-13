static rfbBool rectSwapIfLEAndClip(uint16_t* x,uint16_t* y,uint16_t* w,uint16_t* h,
		rfbClientPtr cl)
{
	int x1=Swap16IfLE(*x);
	int y1=Swap16IfLE(*y);
	int w1=Swap16IfLE(*w);
	int h1=Swap16IfLE(*h);

	rfbScaledCorrection(cl->scaledScreen, cl->screen, &x1, &y1, &w1, &h1, "rectSwapIfLEAndClip");
	*x = x1;
	*y = y1;
	*w = w1;
	*h = h1;

	if(*w>cl->screen->width-*x)
		*w=cl->screen->width-*x;
	/* possible underflow */
	if(*w>cl->screen->width-*x)
		return FALSE;
	if(*h>cl->screen->height-*y)
		*h=cl->screen->height-*y;
	if(*h>cl->screen->height-*y)
		return FALSE;

	return TRUE;
}