mysql_autodetect_character_set(MYSQL *mysql)
{
  const char *csname= MYSQL_DEFAULT_CHARSET_NAME;

#ifdef __WIN__
  char cpbuf[64];
  {
    UINT cp= GetConsoleCP();
    if (cp == 0)
      cp= GetACP();
    my_snprintf(cpbuf, sizeof(cpbuf), "cp%d", (int)cp);
    csname= my_os_charset_to_mysql_charset(cpbuf);
  }
#elif defined(HAVE_SETLOCALE) && defined(HAVE_NL_LANGINFO)
  {
    if (setlocale(LC_CTYPE, "") && (csname= nl_langinfo(CODESET)))
      csname= my_os_charset_to_mysql_charset(csname);
  }
#endif

  if (mysql->options.charset_name)
    my_free(mysql->options.charset_name);
  if (!(mysql->options.charset_name= my_strdup(csname, MYF(MY_WME))))
    return 1;
  return 0;
}