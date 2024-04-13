mode_from_header (const char *p, size_t s, bool *hbits)
{
  intmax_t u = from_header (p, s, "mode_t",
			    INTMAX_MIN, UINTMAX_MAX,
			    false, false);
  mode_t mode = ((u & TSUID ? S_ISUID : 0)
		 | (u & TSGID ? S_ISGID : 0)
		 | (u & TSVTX ? S_ISVTX : 0)
		 | (u & TUREAD ? S_IRUSR : 0)
		 | (u & TUWRITE ? S_IWUSR : 0)
		 | (u & TUEXEC ? S_IXUSR : 0)
		 | (u & TGREAD ? S_IRGRP : 0)
		 | (u & TGWRITE ? S_IWGRP : 0)
		 | (u & TGEXEC ? S_IXGRP : 0)
		 | (u & TOREAD ? S_IROTH : 0)
		 | (u & TOWRITE ? S_IWOTH : 0)
		 | (u & TOEXEC ? S_IXOTH : 0));
  *hbits = (u & ~07777) != 0;
  return mode;
}