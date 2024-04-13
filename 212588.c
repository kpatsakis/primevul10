  void shrinkBy(int decrease) {
    p -= decrease;
    assertx(p >= begin);
  }