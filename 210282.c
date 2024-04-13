bool Decoder::canSplitFunctionOn(llvm::BasicBlock* bb)
{
	for (auto* u : bb->users())
	{
		// All users must be unconditional branch instructions.
		//
		auto* br = dyn_cast<BranchInst>(u);
		if (br == nullptr || br->isConditional())
		{
			LOG << "\t\t\t\t\t\t" << "!CAN : user not uncond for "
					<< llvmObjToString(u)
					<< ", user = " << llvmObjToString(br) << std::endl;
			return false;
		}

		// Branch can not come from istruction right before basic block.
		// This expects that such branches were created
		// TODO: if
		//
		AsmInstruction brAsm(br);
		AsmInstruction bbAsm(bb);
		if (brAsm.getEndAddress() == bbAsm.getAddress())
		{
			LOG << "\t\t\t\t\t\t" << "branch from ASM insn right before: "
					<< brAsm.getAddress() << " -> " << bbAsm.getAddress()
					<< std::endl;
			return false;
		}

		// BB must be true branch in all users.
		//
//		if (br->getSuccessor(0) != bb)
//		{
//			return false;
//		}
	}

	return true;
}