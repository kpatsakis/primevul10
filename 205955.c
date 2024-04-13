bool Item_field::send(Protocol *protocol, String *buffer)
{
  return protocol->store(result_field);
}