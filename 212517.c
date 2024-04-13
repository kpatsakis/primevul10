sysServices_handler(snmp_varbind_t *varbind, snmp_oid_t *oid)
{
  snmp_api_set_time_ticks(varbind, oid, clock_seconds() * 100);
}