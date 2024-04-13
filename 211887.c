static bool add_oid_section(struct php_x509_request *req) {
  char *str = CONF_get_string(req->req_config, nullptr, "oid_section");
  if (str == nullptr) {
    return true;
  }

  STACK_OF(CONF_VALUE) *sktmp = CONF_get_section(req->req_config, str);
  if (sktmp == nullptr) {
    raise_warning("problem loading oid section %s", str);
    return false;
  }

  for (int i = 0; i < sk_CONF_VALUE_num(sktmp); i++) {
    CONF_VALUE *cnf = sk_CONF_VALUE_value(sktmp, i);
    if (OBJ_sn2nid(cnf->name) == NID_undef &&
      OBJ_ln2nid(cnf->name) == NID_undef &&
      OBJ_create(cnf->value, cnf->name, cnf->name) == NID_undef) {
      raise_warning("problem creating object %s=%s", cnf->name, cnf->value);
      return false;
    }
  }
  return true;
}