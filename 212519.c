snmp_engine_get_next(snmp_header_t *header, snmp_varbind_t *varbinds)
{
  snmp_mib_resource_t *resource;
  uint8_t i;

  i = 0;
  while(varbinds[i].value_type != BER_DATA_TYPE_EOC && i < SNMP_MAX_NR_VALUES) {
    resource = snmp_mib_find_next(&varbinds[i].oid);
    if(!resource) {
      switch(header->version) {
      case SNMP_VERSION_1:
        header->error_status = SNMP_STATUS_NO_SUCH_NAME;
        /*
         * Varbinds are 1 indexed
         */
        header->error_index = i + 1;
        break;
      case SNMP_VERSION_2C:
        (&varbinds[i])->value_type = BER_DATA_TYPE_END_OF_MIB_VIEW;
        break;
      default:
        header->error_status = SNMP_STATUS_NO_SUCH_NAME;
        header->error_index = 0;
      }
    } else {
      resource->handler(&varbinds[i], &resource->oid);
    }

    i++;
  }

  return 0;
}