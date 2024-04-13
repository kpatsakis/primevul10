TEST(BasicFlatBufferModel, TestWithNumThreads) {
  TestErrorReporter reporter;
  auto model = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/test_model.bin", &reporter);
  ASSERT_TRUE(model);
  TrivialResolver resolver(&dummy_reg);
  InterpreterBuilder builder(*model, resolver);

  std::unique_ptr<Interpreter> interpreter;
  ASSERT_EQ(builder(&interpreter, /*num_threads=*/42), kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
  ASSERT_EQ(interpreter->subgraph(0)->context()->recommended_num_threads, 42);

  interpreter.reset();
  ASSERT_EQ(builder(&interpreter, 0), kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
  ASSERT_EQ(interpreter->subgraph(0)->context()->recommended_num_threads, 1);

  interpreter.reset();
  ASSERT_EQ(builder(&interpreter, -1), kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
  ASSERT_EQ(interpreter->subgraph(0)->context()->recommended_num_threads, -1);

  ASSERT_EQ(reporter.num_calls(), 0);
  interpreter.reset(new Interpreter);
  ASSERT_EQ(builder(&interpreter, -2), kTfLiteError);
  ASSERT_EQ(interpreter, nullptr);
  ASSERT_EQ(reporter.num_calls(), 1);
  ASSERT_PRED_FORMAT2(testing::IsSubstring,
                      "num_threads should be >= 0 or just -1",
                      reporter.error_messages());
}