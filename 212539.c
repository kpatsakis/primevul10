snmp_init()
{
  snmp_mib_init();
  process_start(&snmp_process, NULL);
}