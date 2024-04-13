  void open(const char* dir, const char* name, const char* ext)
  {
    DBUG_ENTER("LogFile::open");
    DBUG_PRINT("enter", ("dir: '%s', name: '%s'", dir, name));
    if (!name)
    {
      m_file= stdout;
      DBUG_VOID_RETURN;
    }

    fn_format(m_file_name, name, dir, ext,
              *dir ? MY_REPLACE_DIR | MY_REPLACE_EXT :
              MY_REPLACE_EXT);

    DBUG_PRINT("info", ("file_name: %s", m_file_name));

    if ((m_file= fopen(m_file_name, "wb+")) == NULL)
      die("Failed to open log file %s, errno: %d", m_file_name, errno);

    DBUG_VOID_RETURN;
  }