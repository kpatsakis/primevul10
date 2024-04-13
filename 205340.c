static int diff_check(const char *diff_name)
{
  FILE *res_file;
  char buf[128];
  int have_diff= 0;

  my_snprintf(buf, sizeof(buf), "%s -v", diff_name);

  if (!(res_file= popen(buf, "r")))
    die("popen(\"%s\", \"r\") failed", buf);

  /*
    if diff is not present, nothing will be in stdout to increment
    have_diff
  */
  if (fgets(buf, sizeof(buf), res_file))
    have_diff= 1;

  pclose(res_file);

  return have_diff;
}