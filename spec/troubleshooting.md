# Troubleshooting

## General

### Phi nodes
- Input blocks must be direct predecessor of the phi's block, but for input values it's more permissive.

## Errors
- llvm::LiveVariables::HandleVirtRegUse(unsigned int, llvm::MachineBasicBlock*, llvm::MachineInstr&) () from /usr/lib/x86_64-linux-gnu/libLLVM-8.so.1
=> In a function, a variable from another function is used.