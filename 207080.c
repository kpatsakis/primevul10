send_message (mu_message_t msg)
{
  char *mailer_url = NULL;
  char *sendmail;
  int status;
  
  if (mailvar_get (&sendmail, mailvar_name_sendmail,
		   mailvar_type_string, 0) == 0)
    {
      if (mailvar_is_true (mailvar_name_mailx))
	{
	  /*
	   * Mailx compatibility: assume sendmail:// scheme.
	   */
	  if (!mu_is_proto (sendmail))
	    {
	      status = mu_asprintf (&mailer_url, "sendmail://%s", sendmail);
	      if (status)
		return status;
	      sendmail = mailer_url;
	    }
	}
      
      if (sendmail[0] == '/')
	status = msg_to_pipe (sendmail, msg);
      else
	{
	  mu_mailer_t mailer;
	  
	  status = mu_mailer_create (&mailer, sendmail);
	  if (status == 0)
	    {
	      const char *return_address_str;
	      mu_address_t return_address = NULL;
	      
	      if (mailvar_get (&return_address_str, mailvar_name_return_address,
			       mailvar_type_string, 0) == 0)
		{
		  struct mu_address hint = MU_ADDRESS_HINT_INITIALIZER;
		  status = mu_address_create_hint (&return_address,
						   return_address_str,
						   &hint, 0);
		  if (status)
		    {
		      mu_error (_("invalid return address: %s"),
				mu_strerror (status));
		      mu_mailer_destroy (&mailer);
		      return status;
		    }
		} 

	      if (mailvar_is_true (mailvar_name_verbose))
		{
		  mu_debug_set_category_level (MU_DEBCAT_MAILER,
					  MU_DEBUG_LEVEL_UPTO (MU_DEBUG_PROT));
		}
	      status = mu_mailer_open (mailer, MU_STREAM_RDWR);
	      if (status == 0)
		{
		  status = mu_mailer_send_message (mailer, msg,
						   return_address, NULL);
		  mu_mailer_close (mailer);
		}
	      else
		mu_error (_("Cannot open mailer: %s"),
			  mu_strerror (status));
	      mu_mailer_destroy (&mailer);
	      mu_address_destroy (&return_address);
	    }
	  else
	    mu_error (_("Cannot create mailer: %s"), mu_strerror (status));
	}
    }
  else
    {
      mu_error (_("Variable sendmail not set: no mailer"));
      status = ENOSYS;
    }
  free (mailer_url);
  return status;
}