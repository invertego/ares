#pragma once

namespace nall {

struct shared_memory {
  shared_memory() = default;
  shared_memory(const shared_memory&) = delete;
  auto operator=(const shared_memory&) -> shared_memory& = delete;

  ~shared_memory() {
    reset();
  }

  explicit operator bool() const { return _data; }
  auto size() const -> u32 { return _size; }
  auto acquired() const -> bool { return false; }
  auto acquire() -> u8* { return _data; }
  auto release() -> void {}
  auto reset() -> void;
  auto create(const string& name, u32 size) -> bool;
  auto remove() -> void { reset(); }
  auto open(const string& name, u32 size) -> bool;
  auto close() -> void { reset(); }

  static auto pid() -> u32;

private:
  HANDLE _map = nullptr;
  u8* _data = nullptr;
  u32 _size = 0;
};

}

#if defined(NALL_HEADER_ONLY)
  #include <nall/windows/shared-memory.cpp>
#endif
