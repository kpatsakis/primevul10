TEST_P(JSITest, SymbolTest) {
  if (!rt.global().hasProperty(rt, "Symbol")) {
    // Symbol is an es6 feature which doesn't exist in older VMs.  So
    // the tests which might be elsewhere are all here so they can be
    // skipped.
    return;
  }

  // ObjectTest
  eval("x = {1:2, 'three':Symbol('four')}");
  Object x = rt.global().getPropertyAsObject(rt, "x");
  EXPECT_EQ(x.getPropertyNames(rt).size(rt), 2);
  EXPECT_TRUE(x.hasProperty(rt, "three"));
  EXPECT_EQ(
      x.getProperty(rt, "three").getSymbol(rt).toString(rt), "Symbol(four)");

  // ValueTest
  EXPECT_TRUE(eval("Symbol('sym')").isSymbol());
  EXPECT_EQ(eval("Symbol('sym')").getSymbol(rt).toString(rt), "Symbol(sym)");

  // EqualsTest
  EXPECT_FALSE(Symbol::strictEquals(
      rt,
      eval("Symbol('a')").getSymbol(rt),
      eval("Symbol('a')").getSymbol(rt)));
  EXPECT_TRUE(Symbol::strictEquals(
      rt,
      eval("Symbol.for('a')").getSymbol(rt),
      eval("Symbol.for('a')").getSymbol(rt)));
  EXPECT_FALSE(
      Value::strictEquals(rt, eval("Symbol('a')"), eval("Symbol('a')")));
  EXPECT_TRUE(Value::strictEquals(
      rt, eval("Symbol.for('a')"), eval("Symbol.for('a')")));
  EXPECT_FALSE(Value::strictEquals(rt, eval("Symbol('a')"), eval("'a'")));
}