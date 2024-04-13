bool Item_cache_wrapper::send(Protocol *protocol, String *buffer)
{
  if (result_field)
    return protocol->store(result_field);
  return Item::send(protocol, buffer);
}