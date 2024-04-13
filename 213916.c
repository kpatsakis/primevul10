action_remove_thumb (ExifData *ed, ExifLog *log, ExifParams p)
{
	(void) log;  /* unused */
	(void) p;  /* unused */
	if (ed->data) {
		free (ed->data);
		ed->data = NULL;
	}
	ed->size = 0;
}