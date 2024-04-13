snmp_api_set_oid(snmp_varbind_t *varbind, snmp_oid_t *oid, snmp_oid_t *ret_oid)
{
  memcpy(&varbind->oid, oid, sizeof(snmp_oid_t));
  varbind->value_type = BER_DATA_TYPE_OBJECT_IDENTIFIER;
  memcpy(&varbind->value.oid, ret_oid, sizeof(snmp_oid_t));
}