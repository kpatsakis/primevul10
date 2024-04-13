void read_until_delimiter(DYNAMIC_STRING *ds,
                          DYNAMIC_STRING *ds_delimiter)
{
  char c;
  DBUG_ENTER("read_until_delimiter");
  DBUG_PRINT("enter", ("delimiter: %s, length: %u",
                       ds_delimiter->str, (uint) ds_delimiter->length));

  if (ds_delimiter->length > MAX_DELIMITER_LENGTH)
    die("Max delimiter length(%d) exceeded", MAX_DELIMITER_LENGTH);

  /* Read from file until delimiter is found */
  while (1)
  {
    c= my_getc(cur_file->file);

    if (c == '\n')
    {
      cur_file->lineno++;

      /* Skip newline from the same line as the command */
      if (start_lineno == (cur_file->lineno - 1))
        continue;
    }
    else if (start_lineno == cur_file->lineno)
    {
      /*
        No characters except \n are allowed on
        the same line as the command
      */
      report_or_die("Trailing characters found after command");
    }

    if (feof(cur_file->file))
      report_or_die("End of file encountered before '%s' delimiter was found",
                    ds_delimiter->str);

    if (match_delimiter(c, ds_delimiter->str, ds_delimiter->length))
    {
      DBUG_PRINT("exit", ("Found delimiter '%s'", ds_delimiter->str));
      break;
    }
    dynstr_append_mem(ds, (const char*)&c, 1);
  }
  DBUG_PRINT("exit", ("ds: %s", ds->str));
  DBUG_VOID_RETURN;
}