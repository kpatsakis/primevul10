int csh_like_shell(void)
{
  return strstr((char *)path_tail(p_sh), "csh") != NULL;
}