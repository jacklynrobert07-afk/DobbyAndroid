#pragma once

#include <sys/types.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/mman.h>
#include "pac_kit.h"

// Hapus include platform.h untuk memecah circular dependency
// #include "../source/PlatformUnifiedInterface/platform.h"

// Memastikan fungsi ALIGN_FLOOR tetap terbaca
#include "../source/dobby/utility_macro.h"

namespace features {

template <typename T> inline T arm_thumb_fix_addr(T &addr) {
#if defined(__arm__) || defined(__aarch64__)
  addr = (T)((uintptr_t)addr & ~1);
#endif
  return addr;
}

namespace apple {
template <typename T> inline T arm64e_pac_strip(T &addr) {
  return pac_strip(addr);
}

template <typename T> inline T arm64e_pac_sign(T &addr) {
  return pac_sign(addr);
}

template <typename T> inline T arm64e_pac_strip_and_sign(T &addr) {
  return pac_strip_and_sign(addr);
}
} // namespace apple

namespace android {
inline void make_memory_readable(void *address, size_t size) {
#if defined(ANDROID)
  // [DIPERBAIKI] Menggunakan fungsi native POSIX mprotect dan sysconf 
  // agar tidak perlu memanggil class OSMemory dari platform.h
  long page_size = sysconf(_SC_PAGESIZE);
  auto page = (void *)ALIGN_FLOOR(address, page_size);
  mprotect(page, page_size, PROT_READ | PROT_EXEC);
#endif
}
} // namespace android
} // namespace features
