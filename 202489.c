lt_dlhandle_map (lt_dlinterface_id iface,
		 int (*func) (lt_dlhandle handle, void *data), void *data)
{
  lt__interface_id *iterator = (lt__interface_id *) iface;
  lt_dlhandle cur = handles;

  assert (iface); /* iface is a required argument */

  while (cur)
    {
      int errorcode = 0;

      /* advance while the interface check fails */
      while (cur && iterator->iface
	     && ((*iterator->iface) (cur, iterator->id_string) != 0))
	{
	  cur = cur->next;
	}

      if ((errorcode = (*func) (cur, data)) != 0)
	return errorcode;
    }

  return 0;
}