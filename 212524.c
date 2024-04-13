snmp_engine_get_bulk(snmp_header_t *header, snmp_varbind_t *varbinds)
{
  snmp_mib_resource_t *resource;
  snmp_oid_t oids[SNMP_MAX_NR_VALUES];
  uint32_t j, original_varbinds_length;
  uint8_t repeater;
  uint8_t i, varbinds_length;

  /*
   * A local copy of the requested oids must be kept since
   *  the varbinds are modified on the fly
   */
  original_varbinds_length = 0;
  while(varbinds[original_varbinds_length].value_type != BER_DATA_TYPE_EOC && original_varbinds_length < SNMP_MAX_NR_VALUES) {
    memcpy(&oids[original_varbinds_length], &varbinds[original_varbinds_length].oid, sizeof(snmp_oid_t));
    original_varbinds_length++;
  }

  varbinds_length = 0;
  for(i = 0; i < original_varbinds_length; i++) {
    if(i >= header->non_repeaters) {
      break;
    }

    resource = snmp_mib_find_next(&oids[i]);
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
      if(varbinds_length < SNMP_MAX_NR_VALUES) {
        resource->handler(&varbinds[varbinds_length], &resource->oid);
        (varbinds_length)++;
      } else {
        return -1;
      }
    }
  }

  for(i = 0; i < header->max_repetitions; i++) {
    repeater = 0;
    for(j = header->non_repeaters; j < original_varbinds_length; j++) {
      resource = snmp_mib_find_next(&oids[j]);
      if(!resource) {
        switch(header->version) {
        case SNMP_VERSION_1:
          header->error_status = SNMP_STATUS_NO_SUCH_NAME;
          /*
           * Varbinds are 1 indexed
           */
          header->error_index = varbinds_length + 1;
          break;
        case SNMP_VERSION_2C:
          if(varbinds_length < SNMP_MAX_NR_VALUES) {
            (&varbinds[varbinds_length])->value_type = BER_DATA_TYPE_END_OF_MIB_VIEW;
            memcpy(&varbinds[varbinds_length].oid, &oids[j], sizeof(snmp_oid_t));
            (varbinds_length)++;
          } else {
            return -1;
          }
          break;
        default:
          header->error_status = SNMP_STATUS_NO_SUCH_NAME;
          header->error_index = 0;
        }
      } else {
        if(varbinds_length < SNMP_MAX_NR_VALUES) {
          resource->handler(&varbinds[varbinds_length], &resource->oid);
          (varbinds_length)++;
          memcpy(&oids[j], &resource->oid, sizeof(snmp_oid_t));
          repeater++;
        } else {
          return -1;
        }
      }
    }
    if(repeater == 0) {
      break;
    }
  }

  return 0;
}