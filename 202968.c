rfbSendCopyRegion(rfbClientPtr cl,
                  sraRegionPtr reg,
                  int dx,
                  int dy)
{
    int x, y, w, h;
    rfbFramebufferUpdateRectHeader rect;
    rfbCopyRect cr;
    sraRectangleIterator* i;
    sraRect rect1;

    /* printf("copyrect: "); sraRgnPrint(reg); putchar('\n');fflush(stdout); */
    i = sraRgnGetReverseIterator(reg,dx>0,dy>0);

    while(sraRgnIteratorNext(i,&rect1)) {
      x = rect1.x1;
      y = rect1.y1;
      w = rect1.x2 - x;
      h = rect1.y2 - y;

      rect.r.x = Swap16IfLE(x);
      rect.r.y = Swap16IfLE(y);
      rect.r.w = Swap16IfLE(w);
      rect.r.h = Swap16IfLE(h);
      rect.encoding = Swap32IfLE(rfbEncodingCopyRect);

      memcpy(&cl->updateBuf[cl->ublen], (char *)&rect,
	     sz_rfbFramebufferUpdateRectHeader);
      cl->ublen += sz_rfbFramebufferUpdateRectHeader;

      cr.srcX = Swap16IfLE(x - dx);
      cr.srcY = Swap16IfLE(y - dy);

      memcpy(&cl->updateBuf[cl->ublen], (char *)&cr, sz_rfbCopyRect);
      cl->ublen += sz_rfbCopyRect;

      cl->rfbRectanglesSent[rfbEncodingCopyRect]++;
      cl->rfbBytesSent[rfbEncodingCopyRect]
	+= sz_rfbFramebufferUpdateRectHeader + sz_rfbCopyRect;

    }

    return TRUE;
}