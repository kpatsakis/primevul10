show_ifd (ExifContent *content, void *data)
{
	exif_content_foreach_entry (content, show_entry_list, data);
}