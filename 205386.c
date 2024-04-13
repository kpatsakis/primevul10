}

void dynstr_append_sorted(DYNAMIC_STRING* ds, DYNAMIC_STRING *ds_input,
                          bool keep_header)
{
  unsigned i;
  char *start= ds_input->str;
  DYNAMIC_ARRAY lines;
  DBUG_ENTER("dynstr_append_sorted");

  if (!*start)
    DBUG_VOID_RETURN;  /* No input */

  my_init_dynamic_array(&lines, sizeof(const char*), 32, 32);

  if (keep_header)
  {
    /* First line is result header, skip past it */
    while (*start && *start != '\n')
      start++;
    start++; /* Skip past \n */
    dynstr_append_mem(ds, ds_input->str, start - ds_input->str);
  }

  /* Insert line(s) in array */
  while (*start)
  {
    char* line_end= (char*)start;

    /* Find end of line */
    while (*line_end && *line_end != '\n')
      line_end++;
    *line_end= 0;

    /* Insert pointer to the line in array */
    if (insert_dynamic(&lines, (uchar*) &start))
      die("Out of memory inserting lines to sort");

    start= line_end+1;
  }

  /* Sort array */
  qsort(lines.buffer, lines.elements,
        sizeof(char**), (qsort_cmp)comp_lines);

  /* Create new result */
  for (i= 0; i < lines.elements ; i++)
  {
    const char **line= dynamic_element(&lines, i, const char**);
    dynstr_append(ds, *line);
    dynstr_append(ds, "\n");
  }

  delete_dynamic(&lines);