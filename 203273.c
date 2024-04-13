process_cmd_retries(const char *line)
{
  int retries;

  retries = atoi(line);
  if (retries < 0) {
    fprintf(stderr, "Invalid maximum number of retries\n");
    return 0;
  }
  max_retries = retries;
  return 1;
}