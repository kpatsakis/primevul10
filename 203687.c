cmd_vector (const char *com _GL_UNUSED, const char *val, void *place)
{
  char ***pvec = (char ***)place;

  if (*val)
    *pvec = merge_vecs (*pvec, sepstring (val));
  else
    {
      free_vec (*pvec);
      *pvec = NULL;
    }
  return true;
}