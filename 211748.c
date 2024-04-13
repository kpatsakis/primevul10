String HHVM_FUNCTION(implode,
                     const Variant& arg1,
                     const Variant& arg2 /* = uninit_variant */) {
  if (isContainer(arg1)) {
    return StringUtil::Implode(arg1, arg2.toString(), false);
  } else if (arg1.isClsMeth() && RO::EvalIsCompatibleClsMethType) {
    raiseClsMethToVecWarningHelper(__FUNCTION__+2);
    auto const clsMeth = arg1.toClsMethVal();
    return concat3(
      StrNR(clsMeth->getCls()->name()),
      arg2.toString(),
      StrNR(clsMeth->getFunc()->name()));
  } else if (isContainer(arg2)) {
    return StringUtil::Implode(arg2, arg1.toString(), false);
  } else if (arg2.isClsMeth() && RO::EvalIsCompatibleClsMethType) {
    raiseClsMethToVecWarningHelper(__FUNCTION__+2);
    auto const clsMeth = arg2.toClsMethVal();
    return concat3(
      StrNR(clsMeth->getCls()->name()),
      arg1.toString(),
      StrNR(clsMeth->getFunc()->name()));
  } else {
    raise_bad_type_warning("implode() expects a container as "
                             "one of the arguments");
    return String();
  }
}