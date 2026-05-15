#include "platform_detect_macro.h"
#if defined(TARGET_ARCH_ARM)

#include "dobby/dobby_internal.h"

#include "core/assembler/assembler-arm.h"

#include "TrampolineBridge/ClosureTrampolineBridge/ClosureTrampoline.h"
#include "TrampolineBridge/ClosureTrampolineBridge/common_bridge_handler.h"

using namespace zz;
using namespace zz::arm;

// [DIPERBAIKI] Mengubah ClosureTrampolineEntry menjadi ClosureTrampoline sesuai header
ClosureTrampoline *ClosureTrampoline::CreateClosureTrampoline(void *carry_data, void *carry_handler) {

#ifdef ENABLE_CLOSURE_TRAMPOLINE_TEMPLATE
#define CLOSURE_TRAMPOLINE_SIZE (7 * 4)
  // use closure trampoline template code, find the executable memory and patch it.
  auto code = AssemblerCodeBuilder::FinalizeCodeFromAddress(closure_trampoline_template, CLOSURE_TRAMPOLINE_SIZE);
  // Implementation note: The template path is deprecated and likely incomplete.
  return nullptr; 
#else

#define _ turbo_assembler_.
  TurboAssembler turbo_assembler_(0);

  PseudoLabel entry_label(0);
  PseudoLabel forward_bridge_label(0);

  _ Ldr(r12, &entry_label);
  _ Ldr(pc, &forward_bridge_label);
  _ bindLabel(&entry_label);
  
  // Trick to resolve placeholder address
  _ EmitAddress((uint32_t)(uintptr_t)0xdeadbeef); 
  uint32_t data_offset = turbo_assembler_.code_buffer()->size() - 4;

  _ bindLabel(&forward_bridge_label);
  _ EmitAddress((uint32_t)(uintptr_t)get_closure_bridge_addr());

  // [DIPERBAIKI] Menggunakan AssemblerCodeBuilder yang baru
  auto closure_tramp_block = AssemblerCodeBuilder::FinalizeFromTurboAssembler(&turbo_assembler_);
  
  // Patch the deadbeef with the real carry_data address later
  
  CodeMemBlock mem_block(closure_tramp_block.addr(), closure_tramp_block.size());
  
  // [DIPERBAIKI] Membuat instance ClosureTrampoline dengan format baru
  ClosureTrampoline *tramp_entry = new ClosureTrampoline(0, mem_block, carry_data, carry_handler);
  
  // Final patch
  void *real_data_ptr = (void *)((addr_t)tramp_entry);
  DobbyCodePatch((void *)(closure_tramp_block.addr() + data_offset), (uint8_t *)&real_data_ptr, sizeof(void*));

  return tramp_entry;
#endif
}

#endif
