snmp_api_set_time_ticks(snmp_varbind_t *varbind, snmp_oid_t *oid, uint32_t integer)
{
  memcpy(&varbind->oid, oid, sizeof(snmp_oid_t));
  varbind->value_type = BER_DATA_TYPE_TIMETICKS;
  varbind->value.integer = integer;
}