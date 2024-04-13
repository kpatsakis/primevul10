String HHVM_FUNCTION(chr, const Variant& ascii) {
  switch (ascii.getType()) {
    case KindOfBoolean:
    case KindOfInt64:
    case KindOfDouble:
      return String::FromChar(ascii.toInt64() & 0xFF);
    case KindOfPersistentString:
    case KindOfString:
      return String::FromChar(
        ascii.toString().isNumeric() ? ascii.toInt64() & 0xFF : 0);
    case KindOfNull:
    case KindOfUninit:
    case KindOfPersistentVec:
    case KindOfPersistentDict:
    case KindOfPersistentKeyset:
    case KindOfPersistentDArray:
    case KindOfPersistentVArray:
    case KindOfVec:
    case KindOfDict:
    case KindOfKeyset:
    case KindOfDArray:
    case KindOfVArray:
    case KindOfObject:
    case KindOfResource:
    case KindOfRFunc:
    case KindOfFunc:
    case KindOfClass:
    case KindOfLazyClass:
    case KindOfClsMeth:
    case KindOfRClsMeth:
    case KindOfRecord:
      return String::FromChar(0);
  }

  not_reached();
}