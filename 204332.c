minor_from_header (const char *p, size_t s)
{
  return from_header (p, s, "minor_t",
		      TYPE_MINIMUM (minor_t), TYPE_MAXIMUM (minor_t),
		      false, false);
}