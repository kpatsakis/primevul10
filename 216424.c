TEST(BasicFlatBufferModel, TestHandleModelWithWhileOpContainsForwardingInput) {
  const auto model_path =
      "tensorflow/lite/testdata/while_op_with_forwarding_input.bin";

  std::unique_ptr<tflite::FlatBufferModel> model =
      FlatBufferModel::BuildFromFile(model_path);
  ASSERT_NE(model, nullptr);

  tflite::ops::builtin::BuiltinOpResolver resolver;
  InterpreterBuilder builder(*model, resolver);
  std::unique_ptr<Interpreter> interpreter;
  ASSERT_EQ(builder(&interpreter), kTfLiteOk);
  ASSERT_NE(interpreter, nullptr);
  ASSERT_EQ(interpreter->AllocateTensors(), kTfLiteOk);

  int32_t* tensor_data = interpreter->typed_tensor<int32_t>(0);
  tensor_data[0] = 20;

  auto tensor = interpreter->tensor(1);
  DynamicBuffer buf;
  buf.AddString("a", 1);
  buf.WriteToTensor(tensor, /*new_shape=*/nullptr);

  ASSERT_EQ(interpreter->Invoke(), kTfLiteOk);
}