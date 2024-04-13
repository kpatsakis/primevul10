time_from_header (const char *p, size_t s)
{
  return from_header (p, s, "time_t",
		      TYPE_MINIMUM (time_t), TYPE_MAXIMUM (time_t),
		      false, false);
}