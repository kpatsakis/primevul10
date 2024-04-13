dequote_pathname (pathname)
     char *pathname;
{
  if (MB_CUR_MAX > 1)
    wdequote_pathname (pathname);
  else
    udequote_pathname (pathname);
}