snmp_mib_cmp_oid(snmp_oid_t *oid1, snmp_oid_t *oid2)
{
  uint8_t i;

  i = 0;
  while(i < oid1->length && i < oid2->length) {
    if(oid1->data[i] != oid2->data[i]) {
      if(oid1->data[i] < oid2->data[i]) {
        return -1;
      }
      return 1;
    }
    i++;
  }

  if(i == oid1->length &&
     i < oid2->length) {
    return -1;
  }

  if(i < oid1->length &&
     i == oid2->length) {
    return 1;
  }

  return 0;
}