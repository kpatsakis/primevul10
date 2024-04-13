*/
MYSQL_PLUGIN_EXPORT
char *mysql_authentication_dialog_ask(MYSQL *mysql, int type,
                                      const char *prompt,
                                      char *buf, int buf_len)
{
  char *s=buf;

  fputs(prompt, stdout);
  fputs(" ", stdout);

  if (!fgets(buf, buf_len-1, stdin))
    buf[0]= 0;
  else if (buf[0] && (s= strend(buf))[-1] == '\n')
    s[-1]= 0;

  for (s= buf; *s; s++)
    fputc(type == 2 ? '*' : *s, stdout);

  fputc('\n', stdout);
