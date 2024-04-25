#include "X86.h"
#include "X86InstrInfo.h"
#include "X86Subtarget.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Pass.h"
#include <vector>

using namespace llvm;

namespace {

class X86KostinPass : public MachineFunctionPass {
public:
  static char ID;
  X86KostinPass() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

private:
  const TargetInstrInfo *TII;
  std::vector<std::pair<MachineInstr *, MachineInstr *>> ToReplace;

  void identifyReplaceableInstructions(MachineFunction &MF);
  void replaceInstructions(MachineFunction &MF);

  bool canCombine(MachineInstr &MI1, MachineInstr &MI2);
};

char X86KostinPass::ID = 0;

bool X86KostinPass::runOnMachineFunction(MachineFunction &MF) {
  TII = MF.getSubtarget().getInstrInfo();
  identifyReplaceableInstructions(MF);
  if (ToReplace.empty())
    return false;
  replaceInstructions(MF);
  return true;
}

void X86KostinPass::identifyReplaceableInstructions(MachineFunction &MF) {
  for (auto &MBB : MF) {
    for (auto MI = MBB.begin(), E = MBB.end(); MI != E; ++MI) {
      auto &CurrentMI = *MI;
      auto NextMI = std::next(MI, 1);
      if (NextMI != E && canCombine(CurrentMI, *NextMI)) {
        ToReplace.emplace_back(&CurrentMI, &*NextMI);
      }
    }
  }
}

bool X86KostinPass::canCombine(MachineInstr &MI1, MachineInstr &MI2) {
  if ((MI1.getOpcode() == X86::MULPDrr && MI2.getOpcode() == X86::ADDPDrr) ||
      (MI1.getOpcode() == X86::ADDPDrr && MI2.getOpcode() == X86::MULPDrr)) {
    for (const MachineOperand &MO : MI2.operands()) {
      if (MO.isReg() && MO.isUse() &&
          MO.getReg() == MI1.getOperand(0).getReg()) {
        return true;
      }
    }
  }
  return false;
}

void X86KostinPass::replaceInstructions(MachineFunction &MF) {
  for (auto &Pair : ToReplace) {
    MachineInstr *MI1 = Pair.first;
    MachineInstr *MI2 = Pair.second;

    unsigned Opcode = X86::VFMADD213PDZ128r;
    unsigned Op1 = MI1->getOperand(1).getReg();
    unsigned Op2 = MI1->getOperand(2).getReg();
    unsigned Op3 = MI2->getOperand(2).getReg();
    if (MI1->getOpcode() == X86::ADDPDrr) {
      std::swap(Op1, Op3);
    }

    BuildMI(*MI1->getParent(), *MI1, MI1->getDebugLoc(), TII->get(Opcode))
        .addReg(MI2->getOperand(0).getReg(), RegState::Define)
        .addReg(Op1)
        .addReg(Op2)
        .addReg(Op3);

    MI1->eraseFromParent();
    MI2->eraseFromParent();
  }
}

} // namespace

static RegisterPass<X86KostinPass> X("x86-kostin-pass", "X86 Kostin Pass",
                                     false, false);
