	__acquires(proto_list_lock)
{
	read_lock(&proto_list_lock);
	return seq_list_start_head(&proto_list, *pos);
}