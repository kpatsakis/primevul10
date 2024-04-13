lt_dlhandle_iterate (lt_dlinterface_id iface, lt_dlhandle place)
{
  lt_dlhandle handle = place;
  lt__interface_id *iterator = (lt__interface_id *) iface;

  assert (iface); /* iface is a required argument */

  if (!handle)
    handle = handles;
  else
    handle = handle->next;

  /* advance while the interface check fails */
  while (handle && iterator->iface
	 && ((*iterator->iface) (handle, iterator->id_string) != 0))
    {
      handle = handle->next;
    }

  return handle;
}