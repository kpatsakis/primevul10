static void add_assoc_name_entry(Array &ret, const char *key,
                                 X509_NAME *name, bool shortname) {
  Array subitem_data;
  Array &subitem = key ? subitem_data : ret;

  for (int i = 0; i < X509_NAME_entry_count(name); i++) {
    X509_NAME_ENTRY *ne = X509_NAME_get_entry(name, i);
    ASN1_OBJECT *obj = X509_NAME_ENTRY_get_object(ne);
    int nid = OBJ_obj2nid(obj);
    int obj_cnt = 0;

    char *sname;
    if (shortname) {
      sname = (char *)OBJ_nid2sn(nid);
    } else {
      sname = (char *)OBJ_nid2ln(nid);
    }

    Array subentries;
    int last = -1;
    int j;
    ASN1_STRING *str = nullptr;
    unsigned char *to_add = nullptr;
    int to_add_len = 0;
    for (;;) {
      j = X509_NAME_get_index_by_OBJ(name, obj, last);
      if (j < 0) {
        if (last != -1) break;
      } else {
        obj_cnt++;
        ne  = X509_NAME_get_entry(name, j);
        str = X509_NAME_ENTRY_get_data(ne);
        if (ASN1_STRING_type(str) != V_ASN1_UTF8STRING) {
          to_add_len = ASN1_STRING_to_UTF8(&to_add, str);
          if (to_add_len != -1) {
            subentries.append(String((char*)to_add, to_add_len, AttachString));
          }
        } else {
          to_add = ASN1_STRING_data(str);
          to_add_len = ASN1_STRING_length(str);
          subentries.append(String((char*)to_add, to_add_len, CopyString));
        }
      }
      last = j;
    }
    i = last;

    if (obj_cnt > 1) {
      subitem.set(String(sname, CopyString), subentries);
    } else if (obj_cnt && str && to_add_len > -1) {
      // Use the string instance we created above.
      subitem.set(String(sname, CopyString), subentries[0]);
    }
  }

  if (key) {
    ret.set(String(key, CopyString), subitem);
  }
}