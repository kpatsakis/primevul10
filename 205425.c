bool open_and_set_current(const char *name)
{
  FILE *opened= fopen(name, "rb");

  if (!opened)
    return false;

  cur_file++;
  cur_file->file= opened;
  cur_file->file_name= my_strdup(name, MYF(MY_FAE));
  cur_file->lineno=1;
  return true;
}