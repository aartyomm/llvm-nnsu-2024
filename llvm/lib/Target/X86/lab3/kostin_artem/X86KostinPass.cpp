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
    MachineInstr *Mul = nullptr;
    for (auto &MI : MBB) {
      if (MI.getOpcode() == X86::MULPDrr) {
        Mul = &MI;
      } else if (MI.getOpcode() == X86::ADDPDrr && Mul) {
        if (Mul->getOperand(0).getReg() == MI.getOperand(1).getReg()) {
          ToReplace.emplace_back(Mul, &MI);
          Mul = nullptr;
        }
      } else if (MI.definesRegister(Mul ? Mul->getOperand(0).getReg() : Register())) {
        Mul = nullptr;
      }
    }
  }
}

void X86KostinPass::replaceInstructions(MachineFunction &MF) {
  for (auto &Pair : ToReplace) {
    MachineInstr *Mul = Pair.first;
    MachineInstr *Add = Pair.second;
    BuildMI(*Mul->getParent(), *Mul, Mul->getDebugLoc(),
            TII->get(X86::VFMADD213PDZ128r))
        .addReg(Add->getOperand(0).getReg(), RegState::Define)
        .addReg(Mul->getOperand(1).getReg())
        .addReg(Mul->getOperand(2).getReg())
        .addReg(Add->getOperand(2).getReg());
    Mul->eraseFromParent();
    Add->eraseFromParent();
  }
}

} // namespace

static RegisterPass<X86KostinPass> X("x86-kostin-pass", "X86 Kostin Pass",
                                     false, false);
