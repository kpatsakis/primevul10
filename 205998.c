Item_cache_temporal::Item_cache_temporal(enum_field_types field_type_arg):
  Item_cache_int(field_type_arg)
{
  if (mysql_type_to_time_type(cached_field_type) == MYSQL_TIMESTAMP_ERROR)
    cached_field_type= MYSQL_TYPE_DATETIME;
}