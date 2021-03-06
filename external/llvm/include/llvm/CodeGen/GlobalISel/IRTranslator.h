//===-- llvm/CodeGen/GlobalISel/IRTranslator.h - IRTranslator ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This file declares the IRTranslator pass.
/// This pass is responsible for translating LLVM IR into MachineInstr.
/// It uses target hooks to lower the ABI but aside from that, the pass
/// generated code is generic. This is the default translator used for
/// GlobalISel.
///
/// \todo Replace the comments with actual doxygen comments.
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_GLOBALISEL_IRTRANSLATOR_H
#define LLVM_CODEGEN_GLOBALISEL_IRTRANSLATOR_H

#include "Types.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineFunctionPass.h"

namespace llvm {
// Forward declarations.
class BasicBlock;
class CallLowering;
class Constant;
class Instruction;
class MachineBasicBlock;
class MachineFunction;
class MachineInstr;
class MachineRegisterInfo;

// Technically the pass should run on an hypothetical MachineModule,
// since it should translate Global into some sort of MachineGlobal.
// The MachineGlobal should ultimately just be a transfer of ownership of
// the interesting bits that are relevant to represent a global value.
// That being said, we could investigate what would it cost to just duplicate
// the information from the LLVM IR.
// The idea is that ultimately we would be able to free up the memory used
// by the LLVM IR as soon as the translation is over.
class IRTranslator : public MachineFunctionPass {
public:
  static char ID;

private:
  /// Interface used to lower the everything related to calls.
  const CallLowering *CLI;
  /// Mapping of the values of the current LLVM IR function
  /// to the related virtual registers.
  ValueToVReg ValToVReg;
  // Constants are special because when we encounter one,
  // we do not know at first where to insert the definition since
  // this depends on all its uses.
  // Thus, we will insert the sequences to materialize them when
  // we know all their users.
  // In the meantime, just keep it in a set.
  // Note: Constants that end up as immediate in the related instructions,
  // do not appear in that map.
  SmallSetVector<const Constant *, 8> Constants;

  DenseMap<const BasicBlock *, MachineBasicBlock *> BBToMBB;

  /// Methods for translating form LLVM IR to MachineInstr.
  /// \see ::translate for general information on the translate methods.
  /// @{

  /// Translate \p Inst into its corresponding MachineInstr instruction(s).
  /// Insert the newly translated instruction(s) right where the MIRBuilder
  /// is set.
  ///
  /// The general algorithm is:
  /// 1. Look for a virtual register for each operand or
  ///    create one.
  /// 2 Update the ValToVReg accordingly.
  /// 2.alt. For constant arguments, if they are compile time constants,
  ///   produce an immediate in the right operand and do not touch
  ///   ValToReg. Actually we will go with a virtual register for each
  ///   constants because it may be expensive to actually materialize the
  ///   constant. Moreover, if the constant spans on several instructions,
  ///   CSE may not catch them.
  ///   => Update ValToVReg and remember that we saw a constant in Constants.
  ///   We will materialize all the constants in finalize.
  /// Note: we would need to do something so that we can recognize such operand
  ///       as constants.
  /// 3. Create the generic instruction.
  ///
  /// \return true if the translation succeeded.
  bool translate(const Instruction &Inst);

  /// Translate \p Inst into a binary operation \p Opcode.
  /// \pre \p Inst is a binary operation.
  bool translateBinaryOp(unsigned Opcode, const Instruction &Inst);

  /// Translate branch (br) instruction.
  /// \pre \p Inst is a branch instruction.
  bool translateBr(const Instruction &Inst);

  /// Translate return (ret) instruction.
  /// The target needs to implement CallLowering::lowerReturn for
  /// this to succeed.
  /// \pre \p Inst is a return instruction.
  bool translateReturn(const Instruction &Inst);
  /// @}

  // Builder for machine instruction a la IRBuilder.
  // I.e., compared to regular MIBuilder, this one also inserts the instruction
  // in the current block, it can creates block, etc., basically a kind of
  // IRBuilder, but for Machine IR.
  MachineIRBuilder MIRBuilder;

  /// MachineRegisterInfo used to create virtual registers.
  MachineRegisterInfo *MRI;

  // * Insert all the code needed to materialize the constants
  // at the proper place. E.g., Entry block or dominator block
  // of each constant depending on how fancy we want to be.
  // * Clear the different maps.
  void finalize();

  /// Get the VReg that represents \p Val.
  /// If such VReg does not exist, it is created.
  unsigned getOrCreateVReg(const Value &Val);

  /// Get the MachineBasicBlock that represents \p BB.
  /// If such basic block does not exist, it is created.
  MachineBasicBlock &getOrCreateBB(const BasicBlock &BB);

public:
  // Ctor, nothing fancy.
  IRTranslator();

  const char *getPassName() const override {
    return "IRTranslator";
  }

  // Algo:
  //   CallLowering = MF.subtarget.getCallLowering()
  //   F = MF.getParent()
  //   MIRBuilder.reset(MF)
  //   MIRBuilder.getOrCreateBB(F.getEntryBB())
  //   CallLowering->translateArguments(MIRBuilder, F, ValToVReg)
  //   for each bb in F
  //     MIRBuilder.getOrCreateBB(bb)
  //     for each inst in bb
  //       if (!translate(MIRBuilder, inst, ValToVReg, ConstantToSequence))
  //         report_fatal_error(???Don???t know how to translate input");
  //   finalize()
  bool runOnMachineFunction(MachineFunction &MF) override;
};

} // End namespace llvm.
#endif
