void json_set_last_error_code(json_error_codes ec) {
  s_json_parser->error_code = ec;
}