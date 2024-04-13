int main(void)
{
uschar buffer[256];

printf("Testing is_ip_address\n");

while (fgets(CS buffer, sizeof(buffer), stdin) != NULL)
  {
  int offset;
  buffer[Ustrlen(buffer) - 1] = 0;
  printf("%d\n", string_is_ip_address(buffer, NULL));
  printf("%d %d %s\n", string_is_ip_address(buffer, &offset), offset, buffer);
  }

printf("Testing string_nextinlist\n");

while (fgets(CS buffer, sizeof(buffer), stdin) != NULL)
  {
  uschar *list = buffer;
  uschar *lp1, *lp2;
  uschar item[256];
  int sep1 = 0;
  int sep2 = 0;

  if (*list == '<')
    {
    sep1 = sep2 = list[1];
    list += 2;
    }

  lp1 = lp2 = list;
  for (;;)
    {
    uschar *item1 = string_nextinlist(&lp1, &sep1, item, sizeof(item));
    uschar *item2 = string_nextinlist(&lp2, &sep2, NULL, 0);

    if (item1 == NULL && item2 == NULL) break;
    if (item == NULL || item2 == NULL || Ustrcmp(item1, item2) != 0)
      {
      printf("***ERROR\nitem1=\"%s\"\nitem2=\"%s\"\n",
        (item1 == NULL)? "NULL" : CS item1,
        (item2 == NULL)? "NULL" : CS item2);
      break;
      }
    else printf("  \"%s\"\n", CS item1);
    }
  }

/* This is a horrible lash-up, but it serves its purpose. */

printf("Testing string_format\n");

while (fgets(CS buffer, sizeof(buffer), stdin) != NULL)
  {
  void *args[3];
  long long llargs[3];
  double dargs[3];
  int dflag = 0;
  int llflag = 0;
  int n = 0;
  int count;
  int countset = 0;
  uschar format[256];
  uschar outbuf[256];
  uschar *s;
  buffer[Ustrlen(buffer) - 1] = 0;

  s = Ustrchr(buffer, ',');
  if (s == NULL) s = buffer + Ustrlen(buffer);

  Ustrncpy(format, buffer, s - buffer);
  format[s-buffer] = 0;

  if (*s == ',') s++;

  while (*s != 0)
    {
    uschar *ss = s;
    s = Ustrchr(ss, ',');
    if (s == NULL) s = ss + Ustrlen(ss);

    if (isdigit(*ss))
      {
      Ustrncpy(outbuf, ss, s-ss);
      if (Ustrchr(outbuf, '.') != NULL)
        {
        dflag = 1;
        dargs[n++] = Ustrtod(outbuf, NULL);
        }
      else if (Ustrstr(outbuf, "ll") != NULL)
        {
        llflag = 1;
        llargs[n++] = strtoull(CS outbuf, NULL, 10);
        }
      else
        {
        args[n++] = (void *)Uatoi(outbuf);
        }
      }

    else if (Ustrcmp(ss, "*") == 0)
      {
      args[n++] = (void *)(&count);
      countset = 1;
      }

    else
      {
      uschar *sss = malloc(s - ss + 1);
      Ustrncpy(sss, ss, s-ss);
      args[n++] = sss;
      }

    if (*s == ',') s++;
    }

  if (!dflag && !llflag)
    printf("%s\n", string_format(outbuf, sizeof(outbuf), CS format,
      args[0], args[1], args[2])? "True" : "False");

  else if (dflag)
    printf("%s\n", string_format(outbuf, sizeof(outbuf), CS format,
      dargs[0], dargs[1], dargs[2])? "True" : "False");

  else printf("%s\n", string_format(outbuf, sizeof(outbuf), CS format,
    llargs[0], llargs[1], llargs[2])? "True" : "False");

  printf("%s\n", CS outbuf);
  if (countset) printf("count=%d\n", count);
  }

return 0;
}