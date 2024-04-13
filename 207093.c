send_attach_file (int fd,
		  const char *realname,
		  const char *content_filename, const char *content_name,
		  const char *content_type, const char *encoding)
{
  return attlist_attach_file (&attachment_list,
			      fd,
			      realname,
			      content_filename,
			      content_name,
			      content_type,
			      encoding);
}