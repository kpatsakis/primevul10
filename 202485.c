lt_dlcaller_set_data (lt_dlinterface_id key, lt_dlhandle handle, void *data)
{
  int n_elements = 0;
  void *stale = (void *) 0;
  lt_dlhandle cur = handle;
  int i;

  if (cur->interface_data)
    while (cur->interface_data[n_elements].key)
      ++n_elements;

  for (i = 0; i < n_elements; ++i)
    {
      if (cur->interface_data[i].key == key)
	{
	  stale = cur->interface_data[i].data;
	  break;
	}
    }

  /* Ensure that there is enough room in this handle's interface_data
     array to accept a new element (and an empty end marker).  */
  if (i == n_elements)
    {
      lt_interface_data *temp
	= REALLOC (lt_interface_data, cur->interface_data, 2+ n_elements);

      if (!temp)
	{
	  stale = 0;
	  goto done;
	}

      cur->interface_data = temp;

      /* We only need this if we needed to allocate a new interface_data.  */
      cur->interface_data[i].key	= key;
      cur->interface_data[1+ i].key	= 0;
    }

  cur->interface_data[i].data = data;

 done:
  return stale;
}