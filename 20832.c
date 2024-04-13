AddMultiFiles (const char *file_name,
               const char *type_name,
               const char *show_filename,
               struct multi_files **multi_start,
               struct multi_files **multi_current)
{
  struct multi_files *multi;
  struct multi_files *multi_type = NULL;
  struct multi_files *multi_name = NULL;
  multi = (struct multi_files *)malloc(sizeof(struct multi_files));
  if (multi) {
    memset(multi, 0, sizeof(struct multi_files));
    multi->form.option = CURLFORM_FILE;
    multi->form.value = file_name;
  }
  else
    return NULL;

  if (!*multi_start)
    *multi_start = multi;

  if (type_name) {
    multi_type = (struct multi_files *)malloc(sizeof(struct multi_files));
    if (multi_type) {
      memset(multi_type, 0, sizeof(struct multi_files));
      multi_type->form.option = CURLFORM_CONTENTTYPE;
      multi_type->form.value = type_name;
      multi->next = multi_type;

      multi = multi_type;
    }
    else {
      free (multi);
      return NULL;
    }
  }
  if (show_filename) {
    multi_name = (struct multi_files *)malloc(sizeof(struct multi_files));
    if (multi_name) {
      memset(multi_name, 0, sizeof(struct multi_files));
      multi_name->form.option = CURLFORM_FILENAME;
      multi_name->form.value = show_filename;
      multi->next = multi_name;

      multi = multi_name;
    }
    else {
      free (multi);
      return NULL;
    }
  }

  if (*multi_current)
    (*multi_current)->next = multi;

  *multi_current = multi;

  return *multi_current;
}