find_handle (const char *search_path, const char *base_name,
	     lt_dlhandle *phandle, lt_dladvise advise)
{
  if (!search_path)
    return 0;

  if (!foreach_dirinpath (search_path, base_name, find_handle_callback,
			  phandle, advise))
    return 0;

  return phandle;
}