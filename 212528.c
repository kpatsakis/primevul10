snmp_api_set_string(snmp_varbind_t *varbind, snmp_oid_t *oid, char *string)
{
  memcpy(&varbind->oid, oid, sizeof(snmp_oid_t));
  varbind->value_type = BER_DATA_TYPE_OCTET_STRING;
  varbind->value.string.string = string;
  varbind->value.string.length = strlen(string);
}