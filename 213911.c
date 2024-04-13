action_set_value (ExifData *ed, ExifLog *log, ExifParams p)
{
	/* If the entry doesn't exist, create it. */
	ExifEntry *e = action_create_value(ed, log, p.tag, p.ifd);

	/* Now set the value and save the data. */
	convert_arg_to_entry (p.set_value, e, exif_data_get_byte_order (ed), log);
}