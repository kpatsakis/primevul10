snd_seq_oss_delete_client(void)
{
	if (system_client >= 0)
		snd_seq_delete_kernel_client(system_client);

	snd_seq_oss_midi_clear_all();

	return 0;
}