static void deleteScanners(struct single_flow_info *scanners) {
  struct single_flow_info *s, *tmp;
  struct port_flow_info *p, *tmp2;

  HASH_ITER(hh, scanners, s, tmp) {
    HASH_ITER(hh, s->ports, p, tmp2) {
      if(s->ports) HASH_DEL(s->ports, p);
      free(p);
    }
    HASH_DEL(scanners, s);
    free(s);
  }
}