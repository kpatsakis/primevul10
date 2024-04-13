gid_from_header (const char *p, size_t s)
{
  return from_header (p, s, "gid_t",
		      TYPE_MINIMUM (gid_t), TYPE_MAXIMUM (gid_t),
		      false, false);
}