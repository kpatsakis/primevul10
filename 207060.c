msg_to_pipe (const char *cmd, mu_message_t msg)
{
  mu_stream_t progstream, msgstream;
  int status, rc;
  char *argv[4];

  argv[0] = getenv ("SHELL");
  if (!argv[0])
    argv[0] = "/bin/sh";
  argv[1] = "-c";
  argv[2] = (char*) cmd;
  argv[3] = NULL;
  status = mu_prog_stream_create (&progstream,
				  argv[0],
				  3, argv,
				  0, NULL, MU_STREAM_WRITE);
  if (status)
    {
      mu_error (_("Cannot pipe to %s: %s"), cmd, mu_strerror (status));
      return status;
    }

  mu_message_get_streamref (msg, &msgstream);
  status = mu_stream_copy (progstream, msgstream, 0, NULL);
  rc = mu_stream_close (progstream);

  if (status == 0 && rc)
    status = rc;

  mu_stream_destroy (&progstream);
  mu_stream_destroy (&msgstream);
  
  if (status)
    {
      mu_error (_("Sending data to %s failed: %s"), cmd,
		  mu_strerror (status));
    }
  return status;
}