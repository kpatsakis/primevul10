lt_dlopenext (const char *filename)
{
  lt_dlhandle	handle	= 0;
  lt_dladvise	advise;

  if (!lt_dladvise_init (&advise) && !lt_dladvise_ext (&advise))
    handle = lt_dlopenadvise (filename, advise);

  lt_dladvise_destroy (&advise);
  return handle;
}