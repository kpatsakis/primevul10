TEST(BasicFlatBufferModel, TestSetNumThreads) {
  TestErrorReporter reporter;
  auto model = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/test_model.bin", &reporter);
  ASSERT_TRUE(model);
  std::unique_ptr<Interpreter> interpreter;
  TrivialResolver resolver(&dummy_reg);
  InterpreterBuilder builder(*model, resolver);

  ASSERT_EQ(builder.SetNumThreads(42), kTfLiteOk);
  interpreter.reset();
  ASSERT_EQ(builder(&interpreter), kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);

  ASSERT_EQ(builder.SetNumThreads(0), kTfLiteOk);
  interpreter.reset();
  ASSERT_EQ(builder(&interpreter), kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);

  ASSERT_EQ(builder.SetNumThreads(-1), kTfLiteOk);
  interpreter.reset();
  ASSERT_EQ(builder(&interpreter), kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);

  ASSERT_EQ(reporter.num_calls(), 0);
  ASSERT_EQ(builder.SetNumThreads(-2), kTfLiteError);
  interpreter.reset();
  ASSERT_EQ(builder(&interpreter), kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
  ASSERT_EQ(reporter.num_calls(), 1);
  ASSERT_PRED_FORMAT2(testing::IsSubstring,
                      "num_threads should be >= 0 or just -1",
                      reporter.error_messages());
}