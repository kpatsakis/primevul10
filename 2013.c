bool disjointComparisonTypes(Type A, Type B) {
  if (!A.isPrimitive() || !B.isPrimitive())
    return false;

  // Check if types are disjoint.
  return Type::intersectTy(A, B).isNoType();
}