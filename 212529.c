snmp_mib_add(snmp_mib_resource_t *new_resource)
{
  snmp_mib_resource_t *resource;
  uint8_t i;

  for(resource = list_head(snmp_mib);
      resource; resource = resource->next) {

    if(snmp_mib_cmp_oid(&resource->oid, &new_resource->oid) > 0) {
      break;
    }
  }
  if(resource == NULL) {
    list_add(snmp_mib, new_resource);
  } else {
    list_insert(snmp_mib, new_resource, resource);
  }

  if(LOG_DBG_ENABLED) {
    /*
     * We print the entire resource table
     */
    LOG_DBG("Table after insert.\n");
    for(resource = list_head(snmp_mib);
        resource; resource = resource->next) {

      i = 0;
      LOG_DBG("{");
      while(i < resource->oid.length) {
        LOG_DBG_("%lu", (unsigned long)resource->oid.data[i]);
        i++;
        if(i < resource->oid.length) {
          LOG_DBG_(".");
        }
      }
      LOG_DBG_("}\n");
    }
  }
}