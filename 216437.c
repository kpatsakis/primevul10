TEST(BasicFlatBufferModel, TestUnsupportedRecursion) {
  const auto model_path =
      "tensorflow/lite/testdata/unsupported_recursion.bin";

  std::unique_ptr<tflite::FlatBufferModel> model =
      FlatBufferModel::BuildFromFile(model_path);
  ASSERT_NE(model, nullptr);

  tflite::ops::builtin::BuiltinOpResolver resolver;
  InterpreterBuilder builder(*model, resolver);
  std::unique_ptr<Interpreter> interpreter;
  ASSERT_EQ(builder(&interpreter), kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
  ASSERT_NE(interpreter->AllocateTensors(), kTfLiteOk);
}