attlist_add (mu_list_t attlist, char *id, char const *encoding,
	     char const *content_type, char const *content_name,
	     char const *content_filename,
	     mu_stream_t stream, int skip_empty, int disp_inline)
{
  struct atchinfo *aptr;
  int rc;
  
  aptr = mu_alloc (sizeof (*aptr));

  aptr->id = id ? mu_strdup (id) : id;
  aptr->encoding = mu_strdup (encoding);
  attach_set_content_type (aptr,
			   content_type
			     ? content_type : "application/octet-stream");
  aptr->name = content_name ? mu_strdup (content_name) : NULL;
  aptr->filename = content_filename ? mu_strdup (content_filename) : NULL;
  aptr->source = stream;
  if (stream)
    mu_stream_ref (stream);
  aptr->skip_empty = skip_empty;
  aptr->disp_inline = disp_inline;
  rc = mu_list_append (attlist, aptr);
  if (rc)
    {
      mu_diag_funcall (MU_DIAG_ERROR, "mu_list_append", NULL, rc);
      exit (1);
    }
}