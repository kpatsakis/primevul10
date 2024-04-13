process_cmd_timeout(const char *line)
{
  int timeout;

  timeout = atoi(line);
  if (timeout < 100) {
    fprintf(stderr, "Timeout %d is too short\n", timeout);
    return 0;
  }
  initial_timeout = timeout;
  return 1;
}