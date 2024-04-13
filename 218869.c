update_send_new_or_existing_notification_icons(rdpContext* context,
                                               const WINDOW_ORDER_INFO* orderInfo,
                                               const NOTIFY_ICON_STATE_ORDER* iconStateOrder)
{
	wStream* s;
	rdpUpdate* update = context->update;
	BYTE controlFlags = ORDER_SECONDARY | (ORDER_TYPE_WINDOW << 2);
	BOOL versionFieldPresent = FALSE;
	UINT16 orderSize =
	    update_calculate_new_or_existing_notification_icons_order(orderInfo, iconStateOrder);

	update_check_flush(context, orderSize);

	s = update->us;
	if (!s)
		return FALSE;

	if (!Stream_EnsureRemainingCapacity(s, orderSize))
		return FALSE;

	/* Write Hdr */
	Stream_Write_UINT8(s, controlFlags);             /* Header (1 byte) */
	Stream_Write_INT16(s, orderSize);                /* OrderSize (2 bytes) */
	Stream_Write_UINT32(s, orderInfo->fieldFlags);   /* FieldsPresentFlags (4 bytes) */
	Stream_Write_UINT32(s, orderInfo->windowId);     /* WindowID (4 bytes) */
	Stream_Write_UINT32(s, orderInfo->notifyIconId); /* NotifyIconId (4 bytes) */

	/* Write body */
	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_VERSION) != 0)
	{
		versionFieldPresent = TRUE;
		Stream_Write_UINT32(s, iconStateOrder->version);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_TIP) != 0)
	{
		Stream_Write_UINT16(s, iconStateOrder->toolTip.length);
		Stream_Write(s, iconStateOrder->toolTip.string, iconStateOrder->toolTip.length);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_INFO_TIP) != 0)
	{
		NOTIFY_ICON_INFOTIP infoTip = iconStateOrder->infoTip;

		/* info tip should not be sent when version is 0 */
		if (versionFieldPresent && iconStateOrder->version == 0)
			return FALSE;

		Stream_Write_UINT32(s, infoTip.timeout);     /* Timeout (4 bytes) */
		Stream_Write_UINT32(s, infoTip.flags);       /* InfoFlags (4 bytes) */
		Stream_Write_UINT16(s, infoTip.text.length); /* InfoTipText (variable) */
		Stream_Write(s, infoTip.text.string, infoTip.text.length);
		Stream_Write_UINT16(s, infoTip.title.length); /* Title (variable) */
		Stream_Write(s, infoTip.title.string, infoTip.title.length);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_FIELD_NOTIFY_STATE) != 0)
	{
		/* notify state should not be sent when version is 0 */
		if (versionFieldPresent && iconStateOrder->version == 0)
			return FALSE;

		Stream_Write_UINT32(s, iconStateOrder->state);
	}

	if ((orderInfo->fieldFlags & WINDOW_ORDER_ICON) != 0)
	{
		ICON_INFO iconInfo = iconStateOrder->icon;
		Stream_Write_UINT16(s, iconInfo.cacheEntry); /* CacheEntry (2 bytes) */
		Stream_Write_UINT8(s, iconInfo.cacheId);     /* CacheId (1 byte) */
		Stream_Write_UINT8(s, iconInfo.bpp);         /* Bpp (1 byte) */
		Stream_Write_UINT16(s, iconInfo.width);      /* Width (2 bytes) */
		Stream_Write_UINT16(s, iconInfo.height);     /* Height (2 bytes) */

		if (iconInfo.bpp <= 8)
		{
			Stream_Write_UINT16(s, iconInfo.cbColorTable); /* CbColorTable (2 bytes) */
		}

		Stream_Write_UINT16(s, iconInfo.cbBitsMask);             /* CbBitsMask (2 bytes) */
		Stream_Write_UINT16(s, iconInfo.cbBitsColor);            /* CbBitsColor (2 bytes) */
		Stream_Write(s, iconInfo.bitsMask, iconInfo.cbBitsMask); /* BitsMask (variable) */
		orderSize += iconInfo.cbBitsMask;

		if (iconInfo.bpp <= 8)
		{
			Stream_Write(s, iconInfo.colorTable, iconInfo.cbColorTable); /* ColorTable (variable) */
		}

		Stream_Write(s, iconInfo.bitsColor, iconInfo.cbBitsColor); /* BitsColor (variable) */
	}
	else if ((orderInfo->fieldFlags & WINDOW_ORDER_CACHED_ICON) != 0)
	{
		CACHED_ICON_INFO cachedIcon = iconStateOrder->cachedIcon;
		Stream_Write_UINT16(s, cachedIcon.cacheEntry); /* CacheEntry (2 bytes) */
		Stream_Write_UINT8(s, cachedIcon.cacheId);     /* CacheId (1 byte) */
	}

	update->numberOrders++;
	return TRUE;
}