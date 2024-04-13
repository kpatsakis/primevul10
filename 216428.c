TEST(BasicFlatBufferModel, TestSubgraphName) {
  auto m = FlatBufferModel::BuildFromFile(
      "tensorflow/lite/testdata/"
      "2_subgraphs_dont_delegate_name.bin");
  ASSERT_TRUE(m);
  std::unique_ptr<Interpreter> interpreter;
  ASSERT_EQ(InterpreterBuilder(*m, TrivialResolver())(&interpreter), kTfLiteOk);
  EXPECT_EQ(interpreter->subgraphs_size(), 2);
  EXPECT_EQ(interpreter->subgraph(0)->GetName(), "");
  EXPECT_EQ(interpreter->subgraph(1)->GetName(), "VALIDATION:main");
}