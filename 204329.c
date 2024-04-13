major_from_header (const char *p, size_t s)
{
  return from_header (p, s, "major_t",
		      TYPE_MINIMUM (major_t), TYPE_MAXIMUM (major_t),
		      false, false);
}