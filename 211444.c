static bool php_string_to_if_index(const char *val, unsigned *out)
{
#if HAVE_IF_NAMETOINDEX
  unsigned int ind = if_nametoindex(val);
  if (ind == 0) {
    raise_warning("no interface with name \"%s\" could be found", val);
    return false;
  } else {
    *out = ind;
    return true;
  }
#else
  raise_warning("this platform does not support looking up an interface by "
                "name, an integer interface index must be supplied instead");
  return false;
#endif
}