  void write(DYNAMIC_STRING* ds)
  {
    DBUG_ENTER("LogFile::write");
    DBUG_ASSERT(m_file);

    if (ds->length == 0)
      DBUG_VOID_RETURN;
    DBUG_ASSERT(ds->str);

#ifdef EXTRA_DEBUG
    DBUG_PRINT("QQ", ("str: %*s", (int) ds->length, ds->str));
#endif

    if (fwrite(ds->str, 1, ds->length, m_file) != ds->length)
      die("Failed to write %lu bytes to '%s', errno: %d",
          (unsigned long)ds->length, m_file_name, errno);
    m_bytes_written+= ds->length;
    DBUG_VOID_RETURN;
  }