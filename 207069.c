saveatt (void *item, void *data)
{
  struct atchinfo *aptr = item;
  compose_env_t *env = data;
  mu_message_t part;
  int rc;
  
  rc = mu_attachment_create (&part, aptr->content_type, aptr->encoding,
			     aptr->name,
			     aptr->filename);
  if (rc)
    {
      mu_error (_("can't create attachment %s: %s"),
		aptr->id, mu_strerror (rc));
      return 1;
    }
  
  rc = save_attachment (aptr, env, part);
  mu_message_unref (part);
  return rc;
}