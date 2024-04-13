String HHVM_FUNCTION(number_format,
                     double number,
                     int decimals /* = 0 */,
                     const Variant& dec_point_in /* = "." */,
                     const Variant& thousands_sep_in /* = "," */) {

  String dec_point(".");
  if (!dec_point_in.isNull()) {
    dec_point = dec_point_in.toString();
  }
  String thousands_sep(",");
  if (!thousands_sep_in.isNull()) {
    thousands_sep = thousands_sep_in.toString();
  }

  return string_number_format(number, decimals, dec_point, thousands_sep);
}