TEST_P(JSITest, JSErrorTest) {
  // JSError creation can lead to further errors.  Make sure these
  // cases are handled and don't cause weird crashes or other issues.
  //
  // Getting message property can throw

  EXPECT_THROW(
      eval("var GetMessageThrows = {get message() { throw Error('ex'); }};"
           "throw GetMessageThrows;"),
      JSIException);

  EXPECT_THROW(
      eval("var GetMessageThrows = {get message() { throw GetMessageThrows; }};"
           "throw GetMessageThrows;"),
      JSIException);

  // Converting exception message to String can throw

  EXPECT_THROW(
      eval(
          "Object.defineProperty("
          "  globalThis, 'String', {configurable:true, get() { var e = Error(); e.message = 23; throw e; }});"
          "var e = Error();"
          "e.message = 17;"
          "throw e;"),
      JSIException);

  EXPECT_THROW(
      eval(
          "var e = Error();"
          "Object.defineProperty("
          "  e, 'message', {configurable:true, get() { throw Error('getter'); }});"
          "throw e;"),
      JSIException);

  EXPECT_THROW(
      eval("var e = Error();"
           "String = function() { throw Error('ctor'); };"
           "throw e;"),
      JSIException);

  // Converting an exception message to String can return a non-String

  EXPECT_THROW(
      eval("String = function() { return 42; };"
           "var e = Error();"
           "e.message = 17;"
           "throw e;"),
      JSIException);

  // Exception can be non-Object

  EXPECT_THROW(eval("throw 17;"), JSIException);

  EXPECT_THROW(eval("throw undefined;"), JSIException);

  // Converting exception with no message or stack property to String can throw

  EXPECT_THROW(
      eval("var e = {toString() { throw new Error('errstr'); }};"
           "throw e;"),
      JSIException);
}