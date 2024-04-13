get_current_user_info ()
{
  struct passwd *entry;

  /* Don't fetch this more than once. */
  if (current_user.user_name == 0)
    {
#if defined (__TANDEM)
      entry = getpwnam (getlogin ());
#else
      entry = getpwuid (current_user.uid);
#endif
      if (entry)
	{
	  current_user.user_name = savestring (entry->pw_name);
	  current_user.shell = (entry->pw_shell && entry->pw_shell[0])
				? savestring (entry->pw_shell)
				: savestring ("/bin/sh");
	  current_user.home_dir = savestring (entry->pw_dir);
	}
      else
	{
	  current_user.user_name = _("I have no name!");
	  current_user.user_name = savestring (current_user.user_name);
	  current_user.shell = savestring ("/bin/sh");
	  current_user.home_dir = savestring ("/");
	}
#if defined (HAVE_GETPWENT)
      endpwent ();
#endif
    }
}