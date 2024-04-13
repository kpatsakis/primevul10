lt_dlinterface_register (const char *id_string, lt_dlhandle_interface *iface)
{
  lt__interface_id *interface_id = (lt__interface_id *) lt__malloc (sizeof *interface_id);

  /* If lt__malloc fails, it will LT__SETERROR (NO_MEMORY), which
     can then be detected with lt_dlerror() if we return 0.  */
  if (interface_id)
    {
      interface_id->id_string = lt__strdup (id_string);
      if (!interface_id->id_string)
	FREE (interface_id);
      else
	interface_id->iface = iface;
    }

  return (lt_dlinterface_id) interface_id;
}