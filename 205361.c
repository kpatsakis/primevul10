int cat_file(DYNAMIC_STRING* ds, const char* filename)
{
  int fd;
  size_t len;
  char buff[16384];

  if ((fd= my_open(filename, O_RDONLY, MYF(0))) < 0)
    return 1;
  while((len= my_read(fd, (uchar*)&buff,
                      sizeof(buff)-1, MYF(0))) > 0)
  {
    char *p= buff, *start= buff;
    while (p < buff+len)
    {
      /* Convert cr/lf to lf */
      if (*p == '\r' && *(p+1) && *(p+1)== '\n')
      {
        /* Add fake newline instead of cr and output the line */
        *p= '\n';
        p++; /* Step past the "fake" newline */
        *p= 0;
        replace_dynstr_append_mem(ds, start, p-start);
        p++; /* Step past the "fake" newline */
        start= p;
      }
      else
        p++;
    }
    /* Output any chars that migh be left */
    *p= 0;
    replace_dynstr_append_mem(ds, start, p-start);
  }
  my_close(fd, MYF(0));
  return 0;
}