POINTER_POSITION_UPDATE* update_read_pointer_position(rdpUpdate* update, wStream* s)
{
	POINTER_POSITION_UPDATE* pointer_position = calloc(1, sizeof(POINTER_POSITION_UPDATE));

	if (!pointer_position)
		goto fail;

	if (Stream_GetRemainingLength(s) < 4)
		goto fail;

	Stream_Read_UINT16(s, pointer_position->xPos); /* xPos (2 bytes) */
	Stream_Read_UINT16(s, pointer_position->yPos); /* yPos (2 bytes) */
	return pointer_position;
fail:
	free_pointer_position_update(update->context, pointer_position);
	return NULL;
}