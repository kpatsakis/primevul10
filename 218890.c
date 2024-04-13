POINTER_SYSTEM_UPDATE* update_read_pointer_system(rdpUpdate* update, wStream* s)
{
	POINTER_SYSTEM_UPDATE* pointer_system = calloc(1, sizeof(POINTER_SYSTEM_UPDATE));

	if (!pointer_system)
		goto fail;

	if (Stream_GetRemainingLength(s) < 4)
		goto fail;

	Stream_Read_UINT32(s, pointer_system->type); /* systemPointerType (4 bytes) */
	return pointer_system;
fail:
	free_pointer_system_update(update->context, pointer_system);
	return NULL;
}