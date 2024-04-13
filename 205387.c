int read_line(char *buf, int size)
{
  char c, UNINIT_VAR(last_quote), last_char= 0;
  char *p= buf, *buf_end= buf + size - 1;
  int skip_char= 0;
  my_bool have_slash= FALSE;
  
  enum {R_NORMAL, R_Q, R_SLASH_IN_Q,
        R_COMMENT, R_LINE_START} state= R_LINE_START;
  DBUG_ENTER("read_line");

  start_lineno= cur_file->lineno;
  DBUG_PRINT("info", ("Starting to read at lineno: %d", start_lineno));
  for (; p < buf_end ;)
  {
    skip_char= 0;
    c= my_getc(cur_file->file);
    if (feof(cur_file->file))
    {
  found_eof:
      if (cur_file->file != stdin)
      {
	fclose(cur_file->file);
        cur_file->file= 0;
      }
      my_free(cur_file->file_name);
      cur_file->file_name= 0;
      if (cur_file == file_stack)
      {
        /* We're back at the first file, check if
           all { have matching }
        */
        if (cur_block != block_stack)
          die("Missing end of block");

        *p= 0;
        DBUG_PRINT("info", ("end of file at line %d", cur_file->lineno));
        DBUG_RETURN(1);
      }
      cur_file--;
      start_lineno= cur_file->lineno;
      continue;
    }

    if (c == '\n')
    {
      /* Line counting is independent of state */
      cur_file->lineno++;

      /* Convert cr/lf to lf */
      if (p != buf && *(p-1) == '\r')
        p--;
    }

    switch(state) {
    case R_NORMAL:
      if (end_of_query(c))
      {
	*p= 0;
        DBUG_PRINT("exit", ("Found delimiter '%s' at line %d",
                            delimiter, cur_file->lineno));
	DBUG_RETURN(0);
      }
      else if ((c == '{' &&
                (!my_strnncoll_simple(charset_info, (const uchar*) "while", 5,
                                      (uchar*) buf, min(5, p - buf), 0) ||
                 !my_strnncoll_simple(charset_info, (const uchar*) "if", 2,
                                      (uchar*) buf, min(2, p - buf), 0))))
      {
        /* Only if and while commands can be terminated by { */
        *p++= c;
	*p= 0;
        DBUG_PRINT("exit", ("Found '{' indicating start of block at line %d",
                            cur_file->lineno));
	DBUG_RETURN(0);
      }
      else if (c == '\'' || c == '"' || c == '`')
      {
        if (! have_slash) 
        {
	  last_quote= c;
	  state= R_Q;
	}
      }
      have_slash= (c == '\\');
      break;

    case R_COMMENT:
      if (c == '\n')
      {
        /* Comments are terminated by newline */
	*p= 0;
        DBUG_PRINT("exit", ("Found newline in comment at line: %d",
                            cur_file->lineno));
	DBUG_RETURN(0);
      }
      break;

    case R_LINE_START:
      if (c == '#' || c == '-')
      {
        /* A # or - in the first position of the line - this is a comment */
	state = R_COMMENT;
      }
      else if (my_isspace(charset_info, c))
      {
	if (c == '\n')
        {
          if (last_char == '\n')
          {
            /* Two new lines in a row, return empty line */
            DBUG_PRINT("info", ("Found two new lines in a row"));
            *p++= c;
            *p= 0;
            DBUG_RETURN(0);
          }

          /* Query hasn't started yet */
	  start_lineno= cur_file->lineno;
          DBUG_PRINT("info", ("Query hasn't started yet, start_lineno: %d",
                              start_lineno));
        }

        /* Skip all space at begining of line */
	skip_char= 1;
      }
      else if (end_of_query(c))
      {
	*p= 0;
        DBUG_PRINT("exit", ("Found delimiter '%s' at line: %d",
                            delimiter, cur_file->lineno));
	DBUG_RETURN(0);
      }
      else if (c == '}')
      {
        /* A "}" need to be by itself in the begining of a line to terminate */
        *p++= c;
	*p= 0;
        DBUG_PRINT("exit", ("Found '}' in begining of a line at line: %d",
                            cur_file->lineno));
	DBUG_RETURN(0);
      }
      else if (c == '\'' || c == '"' || c == '`')
      {
        last_quote= c;
	state= R_Q;
      }
      else
	state= R_NORMAL;
      break;

    case R_Q:
      if (c == last_quote)
	state= R_NORMAL;
      else if (c == '\\')
	state= R_SLASH_IN_Q;
      break;

    case R_SLASH_IN_Q:
      state= R_Q;
      break;

    }

    last_char= c;

    if (!skip_char)
    {
      /* Could be a multibyte character */
      /* This code is based on the code in "sql_load.cc" */
#ifdef USE_MB
      int charlen = my_mbcharlen(charset_info, (unsigned char) c);
      /* We give up if multibyte character is started but not */
      /* completed before we pass buf_end */
      if ((charlen > 1) && (p + charlen) <= buf_end)
      {
	int i;
	char* mb_start = p;

	*p++ = c;

	for (i= 1; i < charlen; i++)
	{
	  c= my_getc(cur_file->file);
	  if (feof(cur_file->file))
	    goto found_eof;
	  *p++ = c;
	}
	if (! my_ismbchar(charset_info, mb_start, p))
	{
	  /* It was not a multiline char, push back the characters */
	  /* We leave first 'c', i.e. pretend it was a normal char */
	  while (p-1 > mb_start)
	    my_ungetc(*--p);
	}
      }
      else
#endif
	*p++= c;
    }
  }
  die("The input buffer is too small for this query.x\n"      \
      "check your query or increase MAX_QUERY and recompile");
  DBUG_RETURN(0);
}