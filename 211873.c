bool HHVM_FUNCTION(HH_is_list_like, const Variant& val) {
  if (val.isClsMeth() && RO::EvalIsCompatibleClsMethType) {
    raiseClsMethToVecWarningHelper();
    return true;
  }
  auto const& ty = val.getType();
  if (!isArrayLikeType(ty)) return false;
  if (isVecType(ty)) return true;
  auto const& arr = val.asCArrRef();
  return arr->isVectorData();
}