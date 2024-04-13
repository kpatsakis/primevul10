save_attachment (struct atchinfo *aptr, compose_env_t *env, mu_message_t part)
{
  mu_header_t hdr;
  int rc;
  size_t nparts;
  char *p;
  
  rc = mu_attachment_copy_from_stream (part, aptr->source);
  if (rc)
    {
      mu_error (_("cannot attach %s: %s"), aptr->id, mu_strerror (rc));
      return 1;
    }

  if (aptr->skip_empty)
    {
      mu_body_t body;
      size_t size;
      
      rc = mu_message_get_body (part, &body);
      if (rc)
	{
	  mu_diag_funcall (MU_DIAG_ERROR, "mu_message_get_body", aptr->id, rc);
	  return 1;
	}
      rc = mu_body_size (body, &size);
      if (rc)
	{
	  mu_diag_funcall (MU_DIAG_ERROR, "mu_body_size", aptr->id, rc);
	  return 1;
	}
      if (size == 0)
	return 0;
    }
      
  mu_mime_get_num_parts	(env->mime, &nparts);
  mu_message_get_header (part, &hdr);
  mu_rfc2822_msg_id (nparts, &p);
  mu_header_set_value (hdr, MU_HEADER_CONTENT_ID, p, 1);
  free (p);

  if (aptr->disp_inline)
    {
      rc = mu_header_set_value (hdr, MU_HEADER_CONTENT_DISPOSITION,
				"inline",
				1);
      if (rc)
	mu_diag_funcall (MU_DIAG_ERROR, "mu_header_set_value",
			 MU_HEADER_CONTENT_DISPOSITION, rc);
    }
  
  rc = mu_mime_add_part (env->mime, part);
  if (rc)
    {
      mu_diag_funcall (MU_DIAG_ERROR, "mu_mime_add_part", aptr->filename, rc);
      return 1;
    }

  return 0;
}