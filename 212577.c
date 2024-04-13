  void pushArrayData(ArrayData* data) {
    auto const tv = top++;
    tv->m_type = data->toDataType();
    tv->m_data.parr = data;
    assertx(IMPLIES(arrprov::arrayWantsTag(data), arrprov::getTag(data)));
  }