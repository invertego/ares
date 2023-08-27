#include <nall/windows/shared-memory.hpp>

namespace nall {

NALL_HEADER_INLINE auto shared_memory::create(const string& name, u32 size) -> bool {
  reset();

  _map = CreateFileMappingW(
    INVALID_HANDLE_VALUE,
    nullptr,
    PAGE_READWRITE,
    0,
    size,
    utf16_t(name));
  if(!_map) return reset(), false;

  _data = (u8*)MapViewOfFile(
    _map,
    FILE_MAP_ALL_ACCESS,
    0,
    0,
    size);
  if(!_data) return reset(), false;

  _size = size;

  return true;
}

NALL_HEADER_INLINE auto shared_memory::open(const string& name, u32 size) -> bool {
  reset();

  _map = OpenFileMappingW(
    FILE_MAP_ALL_ACCESS,
    false,
    utf16_t(name));
  if(!_map) return reset(), false;

  _data = (u8*)MapViewOfFile(
    _map,
    FILE_MAP_ALL_ACCESS,
    0,
    0,
    size);
  if(!_data) return reset(), false;

  _size = size;

  return true;
}

NALL_HEADER_INLINE auto shared_memory::reset() -> void {
  if(_data) {
    UnmapViewOfFile(_data);
    _data = nullptr;
  }

  if(_map) {
    CloseHandle(_map);
    _map = nullptr;
  }
}

NALL_HEADER_INLINE auto shared_memory::pid() -> u32 {
  return GetCurrentProcessId();
}

}
