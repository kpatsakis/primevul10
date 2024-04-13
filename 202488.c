lt_dlcaller_get_data (lt_dlinterface_id key, lt_dlhandle handle)
{
  void *result = (void *) 0;
  lt_dlhandle cur = handle;

  /* Locate the index of the element with a matching KEY.  */
  if (cur->interface_data)
    {
      int i;
      for (i = 0; cur->interface_data[i].key; ++i)
	{
	  if (cur->interface_data[i].key == key)
	    {
	      result = cur->interface_data[i].data;
	      break;
	    }
	}
    }

  return result;
}