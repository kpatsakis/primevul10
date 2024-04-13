size_t print_file_stack(char *s, const char *end)
{
  char *start= s;
  struct st_test_file* err_file= cur_file;
  if (err_file == file_stack)
    return 0;

  for (;;)
  {
    err_file--;
    s+= my_snprintf(s, end - s, "included from %s at line %d:\n",
                     err_file->file_name, err_file->lineno);
    if (err_file == file_stack)
      break;
  }
  return s - start;
}