//70224 clocks/frame
//  456 clocks/scanline
//  154 scanlines/frame

auto CPU::step() -> void {
  step(1);
}

auto CPU::step(u32 clocks) -> void {
  for(auto n : range(clocks)) {
    if(!r.stop) status.div++;
    if((n4 )status.div == 0) timer262144hz();
    if((n6 )status.div == 0)  timer65536hz();
    if((n8 )status.div == 0)  timer16384hz();
    if((n9 )status.div == 0)   timer8192hz();
    if((n10)status.div == 0)   timer4096hz();
    if((n12)status.div == 0)   timer1024hz();

    Thread::step(1);
    Thread::synchronize();
  }

  if(Model::SuperGameBoy()) {
    system.information.clocksExecuted += clocks;
  }
}

auto CPU::timer262144hz() -> void {
  if(status.timerEnable && status.timerClock == 1) {
    if(++status.tima == 0) {
      status.tima = status.tma;
      raise(Interrupt::Timer);
    }
  }
}

auto CPU::timer65536hz() -> void {
  if(status.timerEnable && status.timerClock == 2) {
    if(++status.tima == 0) {
      status.tima = status.tma;
      raise(Interrupt::Timer);
    }
  }
}

auto CPU::timer16384hz() -> void {
  if(status.timerEnable && status.timerClock == 3) {
    if(++status.tima == 0) {
      status.tima = status.tma;
      raise(Interrupt::Timer);
    }
  }
}

auto CPU::timer8192hz() -> void {
  if(sharedData && sharedIndex >= 0) {
    u32 cycle, other, otherIndex = sharedIndex ^ 1;
    
    sharedData->serialData[sharedIndex] = status.serialData;
    sharedData->serialBits[sharedIndex] = status.serialBits;
    sharedData->serialControl[sharedIndex] = status.serialTransfer << 7 | status.serialClock;

    cycle = ++sharedData->cycle[sharedIndex];
    while((other = sharedData->cycle[otherIndex]) < cycle) spinloop();

    u8 otherData = sharedData->serialData[otherIndex];
    u8 otherControl = sharedData->serialControl[otherIndex];
    u8 otherBits = sharedData->serialBits[otherIndex];

    if(status.serialTransfer && status.serialClock || (otherControl == 0x81)) {
      if(status.serialTransfer && status.serialClock && (otherControl == 0x81)) otherData = 0xff;
      status.serialData <<= 1;
      status.serialData |= otherData >> 7;

      if(!status.serialClock) status.serialBits = otherBits;

      if(--status.serialBits == 0) {
        status.serialTransfer = 0;
        raise(Interrupt::Serial);
      }
    }

    cycle = ++sharedData->cycle[sharedIndex];
    while((other = sharedData->cycle[otherIndex]) < cycle) spinloop();

    return;
  }

  if(status.serialTransfer && status.serialClock) {
    status.serialData <<= 1;
    status.serialData |= 1;
    if(--status.serialBits == 0) {
      status.serialTransfer = 0;
      raise(Interrupt::Serial);
    }
  }
}

auto CPU::timer4096hz() -> void {
  if(status.timerEnable && status.timerClock == 0) {
    if(++status.tima == 0) {
      status.tima = status.tma;
      raise(Interrupt::Timer);
    }
  }
}

auto CPU::timer1024hz() -> void {
  joypPoll();
}

auto CPU::hblank() -> void {
  status.hblankPending = 1;
}

auto CPU::hblankTrigger() -> void {
  if(status.hdmaActive && ppu.status.ly < 144) {
    for(u32 loop : range(16)) {
      writeDMA(status.dmaTarget++, readDMA(status.dmaSource++, 0xff));
      if(loop & 1) step(1 << status.speedDouble);
    }
    if(status.dmaLength-- == 0) {
      status.hdmaActive = 0;
    }
  }
}
