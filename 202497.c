lt_dlhandle_fetch (lt_dlinterface_id iface, const char *module_name)
{
  lt_dlhandle handle = 0;

  assert (iface); /* iface is a required argument */

  while ((handle = lt_dlhandle_iterate (iface, handle)))
    {
      lt_dlhandle cur = handle;
      if (cur && cur->info.name && streq (cur->info.name, module_name))
	break;
    }

  return handle;
}