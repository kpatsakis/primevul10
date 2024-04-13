mail_sendheader (int argc, char **argv)
{
  if (argc == 1)
    mu_list_foreach (add_header_list, list_headers, NULL);
  else if (argc == 2)
    {
      if (strchr (argv[1], ':'))
	send_append_header (argv[1]);
      else
	mu_list_foreach (add_header_list, list_headers, argv[1]);
    }
  else
    {
      size_t len = strlen (argv[1]);
      if (len > 0 && argv[1][len - 1] == ':') 
	argv[1][len - 1] = 0;
      add_header (mu_strdup (argv[1]), mu_strdup (argv[2]), COMPOSE_APPEND);
    }
  return 0;
}