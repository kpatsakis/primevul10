save_dead_message (mu_message_t msg)
{
  if (mailvar_is_true (mailvar_name_save))
    {
      mu_stream_t dead_letter, str;
      int rc;
      time_t t;
      struct tm *tm;
      const char *name = getenv ("DEAD");
      char *sender;
      
      /* FIXME: Use MU_STREAM_APPEND if appenddeadletter, instead of the
	 stream manipulations below */
      rc = mu_file_stream_create (&dead_letter, name,
				  MU_STREAM_CREAT|MU_STREAM_WRITE);
      if (rc)
	{
	  mu_error (_("Cannot open file %s: %s"), name, strerror (rc));
	  return 1;
	}
      if (mailvar_is_true (mailvar_name_appenddeadletter))
	mu_stream_seek (dead_letter, 0, MU_SEEK_END, NULL);
      else
	mu_stream_truncate (dead_letter, 0);

      time (&t);
      tm = gmtime (&t);
      sender = mu_get_user_email (NULL);
      if (!sender)
	sender = mu_strdup ("UNKNOWN");
      mu_stream_printf (dead_letter, "From %s ", sender);
      free (sender);
      mu_c_streamftime (dead_letter, "%c%n", tm, NULL);

      if (mu_message_get_streamref (msg, &str) == 0)
	{
	  mu_stream_copy (dead_letter, str, 0, NULL);
	  mu_stream_unref (str);
	}
      mu_stream_write (dead_letter, "\n", 1, NULL);
      mu_stream_destroy (&dead_letter);
    }
  return 0;
}