attach_set_content_type (struct atchinfo *aptr, char const *content_type)
{
  char *charset;

  if (!content_type)
    content_type = "text/plain";
  if (strncmp (content_type, "text/", 5) == 0
      && !strstr (content_type, "charset=")
      && (charset = util_get_charset ()))
    {
      mu_asprintf (&aptr->content_type, "%s; charset=%s",
		   content_type, charset);
      free (charset);
    }
  else
    aptr->content_type = mu_strdup (content_type);
}