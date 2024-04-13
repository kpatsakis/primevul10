escape_attach (int argc, char **argv, compose_env_t *env)
{
  const char *encoding = default_encoding;
  const char *content_type = default_content_type;
  
  switch (argc)
    {
    case 4:
      encoding = argv[3];
    case 3:
      content_type = argv[2];
    case 2:
      return attlist_attach_file (&env->attlist,
				  -1, argv[1], argv[1], argv[1],
				  content_type, encoding);
    default:
      return escape_check_args (argc, argv, 2, 4);
    }
  return 1;
}