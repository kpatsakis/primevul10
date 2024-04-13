add_attachments (compose_env_t *env, mu_message_t *pmsg)
{
  mu_message_t inmsg, outmsg;
  mu_header_t inhdr, outhdr;
  mu_iterator_t itr;
  int rc;
  
  inmsg = *pmsg;

  if (mailvar_is_true (mailvar_name_mime) && add_body (inmsg, env))
    return 1;
  
  if (mu_list_is_empty (env->attlist))
    return 0;
  
  /* Create a mime object */
  rc = mu_mime_create (&env->mime, NULL,
		       env->alt ?
		         MU_MIME_MULTIPART_ALT : MU_MIME_MULTIPART_MIXED);
  if (rc)
    {
      mu_diag_funcall (MU_DIAG_ERROR, "mu_mime_create", NULL, rc);
      return 1;
    }

  mu_message_get_header (inmsg, &inhdr);

  /* Add the respective attachments */
  rc = mu_list_foreach (env->attlist, saveatt, env);
  if (rc)
    return 1;

  /* Get the resulting message */
  rc = mu_mime_get_message (env->mime, &outmsg);

  if (rc)
    {
      mu_diag_funcall (MU_DIAG_ERROR, "mu_mime_get_message", NULL, rc);
      return 1;
    }

  /* Copy rest of headers from the original message */
  rc = mu_message_get_header (outmsg, &outhdr);
  if (rc)
    {
      mu_diag_funcall (MU_DIAG_ERROR, "mu_message_get_header", NULL, rc);
      return 1;
    }

  rc = mu_header_get_iterator (inhdr, &itr);
  if (rc)
    {
      mu_diag_funcall (MU_DIAG_ERROR, "mu_header_get_iterator", NULL, rc);
      return 1;
    }
  for (mu_iterator_first (itr); !mu_iterator_is_done (itr);
       mu_iterator_next (itr))
    {
      const char *name, *value;
      
      if (mu_iterator_current_kv (itr, (const void **)&name,
				  (void**)&value) == 0)
	{
	  if (mu_c_strcasecmp (name, MU_HEADER_RECEIVED) == 0
	      || mu_c_strncasecmp (name, "X-", 2) == 0)
	    mu_header_append (outhdr, name, value);
	  else if (mu_c_strcasecmp (name, MU_HEADER_MIME_VERSION) == 0 ||
		   mu_c_strncasecmp (name, "Content-", 8) == 0)
	    {
	      mu_error (_("%s: not setting header"), name);
	      continue;
	    }

	  else
	    mu_header_set_value (outhdr, name, value, 1);
	}
    }
  mu_iterator_destroy (&itr);

  mu_message_unref (outmsg);
  mu_message_unref (inmsg);

  *pmsg = outmsg;
  return 0;
}