rfbProcessClientNormalMessage(rfbClientPtr cl)
{
    int n=0;
    rfbClientToServerMsg msg;
    char *str;

    if ((n = ReadExact(cl, (char *)&msg, 1)) <= 0) {
        if (n != 0)
            rfbLogPerror("rfbProcessClientNormalMessage: read");
        rfbCloseClient(cl);
        return;
    }

    switch (msg.type) {

    case rfbSetPixelFormat:

        if ((n = ReadExact(cl, ((char *)&msg) + 1,
                           sz_rfbSetPixelFormatMsg - 1)) <= 0) {
            if (n != 0)
                rfbLogPerror("rfbProcessClientNormalMessage: read");
            rfbCloseClient(cl);
            return;
        }

        cl->format.bitsPerPixel = msg.spf.format.bitsPerPixel;
        cl->format.depth = msg.spf.format.depth;
        cl->format.bigEndian = (msg.spf.format.bigEndian ? TRUE : FALSE);
        cl->format.trueColour = (msg.spf.format.trueColour ? TRUE : FALSE);
        cl->format.redMax = Swap16IfLE(msg.spf.format.redMax);
        cl->format.greenMax = Swap16IfLE(msg.spf.format.greenMax);
        cl->format.blueMax = Swap16IfLE(msg.spf.format.blueMax);
        cl->format.redShift = msg.spf.format.redShift;
        cl->format.greenShift = msg.spf.format.greenShift;
        cl->format.blueShift = msg.spf.format.blueShift;

	cl->readyForSetColourMapEntries = TRUE;

        rfbSetTranslateFunction(cl);

        return;


    case rfbFixColourMapEntries:
        if ((n = ReadExact(cl, ((char *)&msg) + 1,
                           sz_rfbFixColourMapEntriesMsg - 1)) <= 0) {
            if (n != 0)
                rfbLogPerror("rfbProcessClientNormalMessage: read");
            rfbCloseClient(cl);
            return;
        }
        rfbLog("rfbProcessClientNormalMessage: %s",
                "FixColourMapEntries unsupported\n");
        rfbCloseClient(cl);
        return;


    case rfbSetEncodings:
    {
        int i;
        uint32_t enc;
	rfbBool enableCursorShapeUpdates, enableCursorPosUpdates;
	rfbBool useRichCursorEncoding;

        if ((n = ReadExact(cl, ((char *)&msg) + 1,
                           sz_rfbSetEncodingsMsg - 1)) <= 0) {
            if (n != 0)
                rfbLogPerror("rfbProcessClientNormalMessage: read");
            rfbCloseClient(cl);
            return;
        }

        msg.se.nEncodings = Swap16IfLE(msg.se.nEncodings);

	enableCursorShapeUpdates = FALSE;
	enableCursorPosUpdates = FALSE;
	useRichCursorEncoding = FALSE;

        cl->preferredEncoding = -1;
	cl->useCopyRect = FALSE;
	cl->enableLastRectEncoding = FALSE;
        cl->useNewFBSize = FALSE;
#ifdef VINO_HAVE_JPEG
        cl->tightQualityLevel = -1;
#endif

        for (i = 0; i < msg.se.nEncodings; i++) {
            if ((n = ReadExact(cl, (char *)&enc, 4)) <= 0) {
                if (n != 0)
                    rfbLogPerror("rfbProcessClientNormalMessage: read");
                rfbCloseClient(cl);
                return;
            }
            enc = Swap32IfLE(enc);

            switch (enc) {

            case rfbEncodingCopyRect:
		cl->useCopyRect = TRUE;
                break;
            case rfbEncodingRaw:
            case rfbEncodingRRE:
            case rfbEncodingCoRRE:
            case rfbEncodingHextile:
#ifdef VINO_HAVE_ZLIB
	    case rfbEncodingZlib:
            case rfbEncodingZRLE:
#ifdef VINO_HAVE_JPEG
	    case rfbEncodingTight:
#endif
#endif
            /* The first supported encoding is the 'preferred' encoding */
                if (cl->preferredEncoding == -1)
                    cl->preferredEncoding = enc;


                break;
	    case rfbEncodingXCursor:
		enableCursorShapeUpdates = TRUE;
		break;
	    case rfbEncodingRichCursor:
	        enableCursorShapeUpdates = TRUE;
	        useRichCursorEncoding = TRUE;
	        break;
	    case rfbEncodingPointerPos:
		enableCursorPosUpdates = TRUE;
	        break;
	    case rfbEncodingLastRect:
		if (!cl->enableLastRectEncoding) {
		    rfbLog("Enabling LastRect protocol extension for client "
			   "%s\n", cl->host);
		    cl->enableLastRectEncoding = TRUE;
		}
		break;
	    case rfbEncodingNewFBSize:
		if (!cl->useNewFBSize) {
		    rfbLog("Enabling NewFBSize protocol extension for client "
			   "%s\n", cl->host);
		    cl->useNewFBSize = TRUE;
		}
		break;
            default:
#ifdef VINO_HAVE_ZLIB
		if ( enc >= (uint32_t)rfbEncodingCompressLevel0 &&
		     enc <= (uint32_t)rfbEncodingCompressLevel9 ) {
		    cl->zlibCompressLevel = enc & 0x0F;
#ifdef VINO_HAVE_JPEG
		    cl->tightCompressLevel = enc & 0x0F;
		    rfbLog("Using compression level %d for client %s\n",
			   cl->tightCompressLevel, cl->host);
		} else if ( enc >= (uint32_t)rfbEncodingQualityLevel0 &&
			    enc <= (uint32_t)rfbEncodingQualityLevel9 ) {
		    cl->tightQualityLevel = enc & 0x0F;
		    rfbLog("Using image quality level %d for client %s\n",
			   cl->tightQualityLevel, cl->host);
#endif
		} else
#endif
		 rfbLog("rfbProcessClientNormalMessage: ignoring unknown "
                       "encoding type %d\n", (int)enc);
            }
        }

        if (cl->preferredEncoding == -1) {
            cl->preferredEncoding = rfbEncodingRaw;
        }

	if (enableCursorPosUpdates && enableCursorShapeUpdates) {
	  rfbLog("Enabling cursor position and shape (%s encoding) updates for client %s\n",
		 useRichCursorEncoding ? "rich" : "X", cl->host);
	  cl->enableCursorUpdates = TRUE;
	  cl->useRichCursorEncoding = useRichCursorEncoding;
	} else {
	  cl->enableCursorUpdates = FALSE;
	  cl->useRichCursorEncoding = FALSE;
	}

        return;
    }


    case rfbFramebufferUpdateRequest:
    {
        sraRegionPtr tmpRegion;

        if ((n = ReadExact(cl, ((char *)&msg) + 1,
                           sz_rfbFramebufferUpdateRequestMsg-1)) <= 0) {
            if (n != 0)
                rfbLogPerror("rfbProcessClientNormalMessage: read");
            rfbCloseClient(cl);
            return;
        }

	tmpRegion =
	  sraRgnCreateRect(Swap16IfLE(msg.fur.x),
			   Swap16IfLE(msg.fur.y),
			   Swap16IfLE(msg.fur.x)+Swap16IfLE(msg.fur.w),
			   Swap16IfLE(msg.fur.y)+Swap16IfLE(msg.fur.h));

        LOCK(cl->updateMutex);
	sraRgnOr(cl->requestedRegion,tmpRegion);

	if (!cl->readyForSetColourMapEntries) {
	    /* client hasn't sent a SetPixelFormat so is using server's */
	    cl->readyForSetColourMapEntries = TRUE;
	    if (!cl->format.trueColour) {
		if (!rfbSetClientColourMap(cl, 0, 0)) {
		    sraRgnDestroy(tmpRegion);
		    UNLOCK(cl->updateMutex);
		    return;
		}
	    }
	}

       if (!msg.fur.incremental) {
	    sraRgnOr(cl->modifiedRegion,tmpRegion);
	    sraRgnSubtract(cl->copyRegion,tmpRegion);
       }
       UNLOCK(cl->updateMutex);

       sraRgnDestroy(tmpRegion);

       return;
    }

    case rfbKeyEvent:

	cl->rfbKeyEventsRcvd++;

	if ((n = ReadExact(cl, ((char *)&msg) + 1,
			   sz_rfbKeyEventMsg - 1)) <= 0) {
	    if (n != 0)
		rfbLogPerror("rfbProcessClientNormalMessage: read");
	    rfbCloseClient(cl);
	    return;
	}

	if(!cl->viewOnly && cl->screen->kbdAddEvent) {
	    cl->screen->kbdAddEvent(msg.ke.down, (rfbKeySym)Swap32IfLE(msg.ke.key), cl);
	}

        return;


    case rfbPointerEvent:

	cl->rfbPointerEventsRcvd++;

	if ((n = ReadExact(cl, ((char *)&msg) + 1,
			   sz_rfbPointerEventMsg - 1)) <= 0) {
	    if (n != 0)
		rfbLogPerror("rfbProcessClientNormalMessage: read");
	    rfbCloseClient(cl);
	    return;
	}

	if (pointerClient && (pointerClient != cl))
	    return;

	if (msg.pe.buttonMask == 0)
	    pointerClient = NULL;
	else
	    pointerClient = cl;

	if(!cl->viewOnly) {
	    cl->screen->ptrAddEvent(msg.pe.buttonMask,
				    Swap16IfLE(msg.pe.x), Swap16IfLE(msg.pe.y), cl);
	}

        return;


    case rfbClientCutText:

	if ((n = ReadExact(cl, ((char *)&msg) + 1,
			   sz_rfbClientCutTextMsg - 1)) <= 0) {
	    if (n != 0)
		rfbLogPerror("rfbProcessClientNormalMessage: read");
	    rfbCloseClient(cl);
	    return;
	}

	msg.cct.length = Swap32IfLE(msg.cct.length);

	str = (char *)malloc(msg.cct.length);

	if ((n = ReadExact(cl, str, msg.cct.length)) <= 0) {
	    if (n != 0)
	        rfbLogPerror("rfbProcessClientNormalMessage: read");
	    free(str);
	    rfbCloseClient(cl);
	    return;
	}

	if(!cl->viewOnly) {
	    if (cl->screen->setXCutText)
	      cl->screen->setXCutText(str, msg.cct.length, cl);
	}
	free(str);

        return;


    default:

        rfbLog("rfbProcessClientNormalMessage: unknown message type %d\n",
                msg.type);
        rfbLog(" ... closing connection\n");
        rfbCloseClient(cl);
        return;
    }
}