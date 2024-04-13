int64_t HHVM_FUNCTION(levenshtein,
                      const String& str1,
                      const String& str2,
                      int cost_ins /* = 1 */,
                      int cost_rep /* = 1 */,
                      int cost_del /* = 1 */) {
  return string_levenshtein(str1.data(), str1.size(), str2.data(), str2.size(),
                            cost_ins, cost_rep, cost_del);
}