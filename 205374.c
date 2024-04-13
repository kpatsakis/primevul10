  void show_tail(uint lines) {
    DBUG_ENTER("LogFile::show_tail");

    if (!m_file || m_file == stdout)
      DBUG_VOID_RETURN;

    if (lines == 0)
      DBUG_VOID_RETURN;
    lines++;

    int show_offset= 0;
    char buf[256+1];                   /* + zero termination for DBUG_PRINT */
    size_t bytes;
    bool found_bof= false;

    /* Search backward in file until "lines" newline has been found */
    while (lines && !found_bof)
    {
      show_offset-= sizeof(buf)-1;
      while(fseek(m_file, show_offset, SEEK_END) != 0 && show_offset < 0)
      {
        found_bof= true;
        // Seeking before start of file
        show_offset++;
      }

      if ((bytes= fread(buf, 1, sizeof(buf)-1, m_file)) <= 0)
      {
	// ferror=0 will happen here if no queries executed yet
	if (ferror(m_file))
	  fprintf(stderr,
	          "Failed to read from '%s', errno: %d, feof:%d, ferror:%d\n",
	          m_file_name, errno, feof(m_file), ferror(m_file));
        DBUG_VOID_RETURN;
      }

      DBUG_PRINT("info", ("Read %zu bytes from file, buf: %.*s",
                          bytes, (int)bytes, buf));

      char* show_from= buf + bytes;
      while(show_from > buf && lines > 0 )
      {
        show_from--;
        if (*show_from == '\n')
          lines--;
      }
      if (show_from != buf)
      {
        // The last new line was found in this buf, adjust offset
        show_offset+= (show_from - buf) + 1;
        DBUG_PRINT("info", ("adjusted offset to %d", show_offset));
      }
      DBUG_PRINT("info", ("show_offset: %d", show_offset));
    }

    fprintf(stderr, "\nThe result from queries just before the failure was:\n");

    DBUG_PRINT("info", ("show_offset: %d", show_offset));
    if (!lines)
    {
      fprintf(stderr, "< snip >\n");

      if (fseek(m_file, show_offset, SEEK_END) != 0)
      {
        fprintf(stderr, "Failed to seek to position %d in '%s', errno: %d",
                show_offset, m_file_name, errno);
        DBUG_VOID_RETURN;
      }

    }
    else {
      DBUG_PRINT("info", ("Showing the whole file"));
      if (fseek(m_file, 0L, SEEK_SET) != 0)
      {
        fprintf(stderr, "Failed to seek to pos 0 in '%s', errno: %d",
                m_file_name, errno);
        DBUG_VOID_RETURN;
      }
    }

    while ((bytes= fread(buf, 1, sizeof(buf)-1, m_file)) > 0)
      if (bytes != fwrite(buf, 1, bytes, stderr))
        die("Failed to write to '%s', errno: %d",
            m_file_name, errno);

    if (!lines)
    {
      fprintf(stderr,
              "\nMore results from queries before failure can be found in %s\n",
              m_file_name);
    }
    fflush(stderr);

    DBUG_VOID_RETURN;
  }