void lt_dlinterface_free (lt_dlinterface_id key)
{
  lt__interface_id *interface_id = (lt__interface_id *)key;
  FREE (interface_id->id_string);
  FREE (interface_id);
}