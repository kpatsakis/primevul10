static QString currentUser(void)
{
  struct passwd *current = getpwuid(getuid());
  QString fullname(current->pw_gecos);
  if (fullname.find(',') != -1)
    /* Remove everything from and including first comma */
    fullname.resize(fullname.find(','));

  QString username(current->pw_name);

  return fullname + " (" + username + ")";
}