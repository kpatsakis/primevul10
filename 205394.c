void abort_not_supported_test(const char *fmt, ...)
{
  va_list args;
  DBUG_ENTER("abort_not_supported_test");

  /* Print include filestack */
  fflush(stdout);
  fprintf(stderr, "The test '%s' is not supported by this installation\n",
          file_stack->file_name);
  fprintf(stderr, "Detected in file %s at line %d\n",
          cur_file->file_name, cur_file->lineno);

  char buff[DIE_BUFF_SIZE];
  print_file_stack(buff, buff + sizeof(buff));
  fprintf(stderr, "%s", buff);

  /* Print error message */
  va_start(args, fmt);
  if (fmt)
  {
    fprintf(stderr, "reason: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    fflush(stderr);
  }
  va_end(args);

  cleanup_and_exit(62);
}