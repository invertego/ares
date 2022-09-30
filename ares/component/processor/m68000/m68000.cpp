#include <ares/ares.hpp>
#include "m68000.hpp"

namespace ares {

enum : u32  { Byte, Word, Long };
enum : bool { Reverse = 1 };

#include "registers.cpp"
#include "memory.cpp"
#include "effective-address.cpp"
#include "traits.cpp"
#include "conditions.cpp"
#include "algorithms.cpp"
#include "instructions.cpp"
#include "disassembler.cpp"
#include "instruction.cpp"
#include "serialization.cpp"

auto M68000::power() -> void {
  for(auto& dr : r.d) dr = 0;
  for(auto& ar : r.a) ar = 0;
  r.sp = 0;
  r.pc = 0;

  r.c = 0;
  r.v = 0;
  r.z = 0;
  r.n = 0;
  r.x = 0;
  r.i = 7;
  r.s = 1;
  r.t = 0;

  r.irc = 0x4e71;  //nop
  r.ir  = 0x4e71;  //nop
  r.ird = 0x4e71;  //nop

  r.stop  = false;
  r.reset = false;
}

auto M68000::supervisor() -> bool {
  if(r.s) return true;

  r.pc -= 4;
  exception(Exception::Unprivileged, Vector::Unprivileged);
  return false;
}

auto M68000::exception(u32 exception, u32 vector, u32 priority) -> void {
  r.stop  = false;

  // register setup (+6 cyc)
  idle(6);
  n32 pc = r.pc - 4;
  n32 sr = readSR();
  if(!r.s) swap(r.a[7], r.sp);
  r.s = 1;
  r.t = 0;

  // push pc low (+4 cyc)
  push<Word>(pc & 0x0000ffff);

  // external interrupt handling
  if(exception == Exception::Interrupt) {
    // IACK vector number acquisition (+4 cyc normal or +10 to +18 cyc autovectored)
    // & justify vector number (+4 cyc)
    idle(16+4); // assuming autovector cycles (approximated for Megadrive)
    r.i = priority;
  }

  // push pc high & sr (+8 cyc)
  push<Long>(sr << 16 | pc >> 16);

  // read vector address (+8 cyc)
  r.pc = read<Long>(vector << 2);
  // todo: if the vector address read causes an address error, this exception should be escalated

  // prefetch (+8 cyc)
  prefetch();
  if(exception == Exception::Interrupt) idle(2); // dead cycles (+2 cyc)
  prefetch();
}

auto M68000::interrupt(u32 vector, u32 priority) -> void {
  return exception(Exception::Interrupt, vector, priority);
}

}

#include <filesystem>
#include <fstream>

#undef noinline
#include "json.hpp"
using json = nlohmann::json;

struct test_state {
  u32 a[8];
  u32 d[8];
  u32 pc;
  u32 sr;
  u32 usp;
  u32 ssp;
  std::array<u32, 2> prefetch;
  std::vector<std::array<u32, 2>> ram;
};

struct test_case {
  std::string name;
  test_state initial;
  test_state final;
  u32 length;
};

void from_json(const json& j, test_state& s) {
  j.at("a0").get_to(s.a[0]);
  j.at("a1").get_to(s.a[1]);
  j.at("a2").get_to(s.a[2]);
  j.at("a3").get_to(s.a[3]);
  j.at("a4").get_to(s.a[4]);
  j.at("a5").get_to(s.a[5]);
  j.at("a6").get_to(s.a[6]);
  j.at("ssp").get_to(s.a[7]);
  j.at("d0").get_to(s.d[0]);
  j.at("d1").get_to(s.d[1]);
  j.at("d2").get_to(s.d[2]);
  j.at("d3").get_to(s.d[3]);
  j.at("d4").get_to(s.d[4]);
  j.at("d5").get_to(s.d[5]);
  j.at("d6").get_to(s.d[6]);
  j.at("d7").get_to(s.d[7]);
  j.at("pc").get_to(s.pc);
  j.at("sr").get_to(s.sr);
  j.at("usp").get_to(s.usp);
  j.at("prefetch").get_to(s.prefetch);
  j.at("ram").get_to(s.ram);
}

void from_json(const json& j, test_case& t) {
  if(j.empty()) return;
  j.at("name").get_to(t.name);
  j.at("initial").get_to(t.initial);
  j.at("final").get_to(t.final);
  j.at("length").get_to(t.length);
}

struct CPUHarness : ares::M68000 {
  u32 clock = 0;
  std::map<u32, u8> memory;
  CPUHarness();
  auto idle(u32 clocks) -> void override { clock += clocks; }
  auto wait(u32 clocks) -> void override { clock += clocks; }
  auto read(n1 upper, n1 lower, n24 address, n16 _ = 0) -> n16 override {
    n16 data;
    if(lower) data.byte(0) = memory[address | 1];
    if(upper) data.byte(1) = memory[address & ~1];
    return data;
  }
  auto write(n1 upper, n1 lower, n24 address, n16 data) -> void override {
    if(lower) memory[address | 1] = data.byte(0);
    if(upper) memory[address & ~1] = data.byte(1);
  }
};

CPUHarness::CPUHarness() {
  for(const auto& entry : std::filesystem::directory_iterator{ "../tests" }) {
    const auto& path = entry.path();
    if(path.extension() != ".json") continue;
    //if(path.filename() != "ADD.l.json") continue;
    u32 srMask = ~0u;
    if(path.filename() == "ASR.b.json"
    || path.filename() == "ASR.l.json"
    || path.filename() == "ASR.w.json"
    ) srMask &= ~0x11u; //bugged tests
    print(path.filename().string().c_str(), "\n");

    std::ifstream f(path);
    json data = json::parse(f);
    auto tests = data.get<std::vector<test_case>>();
    bool breakOnce = false;

    for(const auto& test : tests) {
      if(test.name.empty()) continue;
      //if(test.name != "ADD d108") continue;
      //if(test.name == "LINK.w 0007") continue;
      if(test.final.pc == 0x1400) continue; //address errors nyi

      power();
      const auto& is = test.initial;
      for(auto i : range(8)) {
        r.a[i] = is.a[i];
        r.d[i] = is.d[i];
      }
      r.sp = is.usp;
      r.pc = is.pc + 4;
      writeSR(is.sr);

      memory.clear();
      const auto& im = is.ram;
      for(auto i : range(im.size())) {
        memory[im[i][0]] = im[i][1];
      }

      r.ir  = is.prefetch[0];
      r.irc = is.prefetch[1];

      clock = 0;
      instruction();

      bool any = false;

      if(!r.s) swap(r.a[7], r.sp);

      const auto& fs = test.final;
      for(auto i : range(8)) {
        if(r.a[i] != fs.a[i]) {
          print("a", i, ": ", hex(r.a[i], 8), ", ", hex(fs.a[i], 8), "\n"); any = true;
        }
        if(r.d[i] != fs.d[i]) {
          print("d", i, ": ", hex(r.d[i], 8), ", ", hex(fs.d[i], 8), "\n"); any = true;
        }
      }
      if(r.sp != fs.usp) {
        print("usp: ", hex(r.sp, 8), ", ", hex(fs.usp, 8), "\n"); any = true;
      }
      if(r.pc != fs.pc + 4) {
        print("pc: ", hex(r.pc, 8), ", ", hex(fs.pc, 8), "\n"); any = true;
      }
      auto sr = readSR();
      if((sr & srMask) != (fs.sr & srMask)) {
        print("sr: ", hex(sr, 8), ", ", hex(fs.sr, 8), "\n"); any = true;
      }
      if(r.ir != fs.prefetch[0]) {
        print("prefetch[0]: ", hex(r.ir, 8), ", ", hex(fs.prefetch[0], 8), "\n"); any = true;
      }
      if(r.irc != fs.prefetch[1]) {
        print("prefetch[1]: ", hex(r.irc, 8), ", ", hex(fs.prefetch[1], 8), "\n"); any = true;
      }
      //if(clock != test.length) {
        //print("length: ", hex(clock, 8), ", ", hex(test.length, 8), "\n"); any = true;
      //}

      const auto& fm = fs.ram;
      for(auto i : range(fm.size())) {
        u32 addr = fm[i][0];
        u8 data = fm[i][1];
        u8 actual = memory[addr];
        if(actual != data) {
          print(hex(addr, 8), ": ", hex(actual, 2), ", ", hex(data, 2), "\n"); any = true;
        }
      }

      if(any) {
        print(test.name.c_str(), /*" | ", disassembleInstruction(is.pc),*/ "\n");
        if(!breakOnce) {
          breakOnce = true;
          break;
        }
      }
    }
  }
}

CPUHarness harness;
