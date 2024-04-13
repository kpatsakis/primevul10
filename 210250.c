const char* jsi_opcode_string(uint opCode)
{
    if (opCode >= (sizeof(jsi_op_names)/sizeof(jsi_op_names[0])))
        return "NULL";
    return jsi_op_names[opCode];
}