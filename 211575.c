void StandardExtension::initVariable() {
  HHVM_FE(is_null);
  HHVM_FE(is_bool);
  HHVM_FE(is_int);
  HHVM_FALIAS(is_integer, is_int);
  HHVM_FALIAS(is_long, is_int);
  HHVM_FE(is_float);
  HHVM_FALIAS(is_double, is_float);
  HHVM_FALIAS(is_real, is_float);
  HHVM_FE(is_numeric);
  HHVM_FE(is_string);
  HHVM_FE(is_scalar);
  HHVM_FE(is_array);
  HHVM_FALIAS(HH\\is_vec, HH_is_vec);
  HHVM_FALIAS(HH\\is_dict, HH_is_dict);
  HHVM_FALIAS(HH\\is_keyset, HH_is_keyset);
  HHVM_FALIAS(HH\\is_varray, HH_is_varray);
  HHVM_FALIAS(HH\\is_darray, HH_is_darray);
  HHVM_FALIAS(HH\\is_vec_or_varray, HH_is_vec_or_varray);
  HHVM_FALIAS(HH\\is_dict_or_darray, HH_is_dict_or_darray);
  HHVM_FALIAS(HH\\is_any_array, HH_is_any_array);
  HHVM_FALIAS(HH\\is_php_array, HH_is_php_array);
  HHVM_FALIAS(HH\\is_list_like, HH_is_list_like);
  HHVM_FALIAS(HH\\is_meth_caller, HH_is_meth_caller);
  HHVM_FE(is_object);
  HHVM_FE(is_resource);
  HHVM_FE(boolval);
  HHVM_FE(intval);
  HHVM_FE(floatval);
  HHVM_FALIAS(doubleval, floatval);
  HHVM_FE(strval);
  HHVM_FE(gettype);
  HHVM_FE(get_resource_type);
  HHVM_FE(print_r);
  HHVM_FE(var_export);
  HHVM_FE(debug_zval_dump);
  HHVM_FE(var_dump);
  HHVM_FE(serialize);
  HHVM_FE(unserialize);
  HHVM_FE(parse_str);
  HHVM_FALIAS(HH\\object_prop_array, HH_object_prop_array);
  HHVM_FALIAS(HH\\serialize_with_options, HH_serialize_with_options);
  HHVM_FALIAS(HH\\Lib\\_Private\\Native\\first, HH_first);
  HHVM_FALIAS(HH\\Lib\\_Private\\Native\\last, HH_last);
  HHVM_FALIAS(HH\\Lib\\_Private\\Native\\first_key, HH_first_key);
  HHVM_FALIAS(HH\\Lib\\_Private\\Native\\last_key, HH_last_key);
  HHVM_FALIAS(HH\\is_late_init_prop_init, HH_is_late_init_prop_init);
  HHVM_FALIAS(HH\\is_late_init_sprop_init, HH_is_late_init_sprop_init);
  HHVM_FALIAS(HH\\global_key_exists, HH_global_key_exists);

  if (RuntimeOption::EnableIntrinsicsExtension) {
    HHVM_FALIAS(__hhvm_intrinsics\\serialize_keep_dvarrays,
                hhvm_intrinsics_serialize_keep_dvarrays);
  }

  loadSystemlib("std_variable");
}