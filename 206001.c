bool Item_direct_view_ref::send(Protocol *protocol, String *buffer)
{
  if (check_null_ref())
    return protocol->store_null();
  return Item_direct_ref::send(protocol, buffer);
}