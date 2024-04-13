uid_from_header (const char *p, size_t s)
{
  return from_header (p, s, "uid_t",
		      TYPE_MINIMUM (uid_t), TYPE_MAXIMUM (uid_t),
		      false, false);
}