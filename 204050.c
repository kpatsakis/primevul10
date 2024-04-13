free_hstat (struct http_stat *hs)
{
  xfree (hs->newloc);
  xfree (hs->remote_time);
  xfree (hs->error);
  xfree (hs->rderrmsg);
  xfree (hs->local_file);
  xfree (hs->orig_file_name);
  xfree (hs->message);
#ifdef HAVE_METALINK
  metalink_delete (hs->metalink);
  hs->metalink = NULL;
#endif
}