bool Item_null::send(Protocol *protocol, String *packet)
{
  return protocol->store_null();
}