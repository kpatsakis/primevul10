add_body (mu_message_t inmsg, compose_env_t *env)
{
  int rc;
  mu_body_t body;
  mu_stream_t str;
  struct atchinfo *aptr;
  
  mu_message_get_body (inmsg, &body);
  mu_body_get_streamref (body, &str);

  aptr = mu_alloc (sizeof (*aptr));
  mu_asprintf (&aptr->id, "(body)");
  aptr->encoding = default_encoding ? mu_strdup (default_encoding) : NULL;
  attach_set_content_type (aptr, default_content_type);
  aptr->name = NULL;
  aptr->filename = NULL;
  aptr->source = str;
  aptr->skip_empty = skip_empty_attachments || multipart_alternative;
  aptr->disp_inline = 1;
  if (!env->attlist)
    env->attlist = attlist_new ();
  rc = mu_list_prepend (env->attlist, aptr);
  if (rc)
    mu_diag_funcall (MU_DIAG_ERROR, "mu_list_prepend", NULL, rc);
  return rc;
}