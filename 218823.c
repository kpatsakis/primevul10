BOOL update_recv_play_sound(rdpUpdate* update, wStream* s)
{
	PLAY_SOUND_UPDATE play_sound;

	if (!update_read_play_sound(s, &play_sound))
		return FALSE;

	return IFCALLRESULT(FALSE, update->PlaySound, update->context, &play_sound);
}