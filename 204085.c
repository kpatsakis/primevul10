rotate_backups(const char *fname)
{
#ifdef __VMS
# define SEP "_"
# define AVS ";*"                       /* All-version suffix. */
# define AVSL (sizeof (AVS) - 1)
#else
# define SEP "."
# define AVSL 0
#endif

  int maxlen = strlen (fname) + sizeof (SEP) + numdigit (opt.backups) + AVSL;
  char *from = alloca (maxlen);
  char *to = alloca (maxlen);
  struct stat sb;
  int i;

  if (stat (fname, &sb) == 0)
    if (S_ISREG (sb.st_mode) == 0)
      return;

  for (i = opt.backups; i > 1; i--)
    {
#ifdef VMS
      /* Delete (all versions of) any existing max-suffix file, to avoid
       * creating multiple versions of it.  (On VMS, rename() will
       * create a new version of an existing destination file, not
       * destroy/overwrite it.)
       */
      if (i == opt.backups)
        {
          snprintf (to, sizeof(to), "%s%s%d%s", fname, SEP, i, AVS);
          delete (to);
        }
#endif
      snprintf (to, maxlen, "%s%s%d", fname, SEP, i);
      snprintf (from, maxlen, "%s%s%d", fname, SEP, i - 1);
      if (rename (from, to))
        logprintf (LOG_NOTQUIET, "Failed to rename %s to %s: (%d) %s\n",
                   from, to, errno, strerror (errno));
    }

  snprintf (to, maxlen, "%s%s%d", fname, SEP, 1);
  if (rename(fname, to))
    logprintf (LOG_NOTQUIET, "Failed to rename %s to %s: (%d) %s\n",
               fname, to, errno, strerror (errno));
}