auto M32X::readExternalIO(n1 upper, n1 lower, n24 address, n16 data) -> n16 {
  //32X ID
  if(address >= 0xa130ec && address <= 0xa130ef) {
    if(address == 0xa130ec && upper) data.byte(1) = 'M';
    if(address == 0xa130ec && lower) data.byte(0) = 'A';
    if(address == 0xa130ee && upper) data.byte(1) = 'R';
    if(address == 0xa130ee && lower) data.byte(0) = 'S';
  }

  //adapter control
  if(address == 0xa15100) {
    data.bit( 0) = io.adapterEnable;
    data.bit( 1) = io.adapterReset;
    data.bit(2, 6) = 0;
    data.bit( 7) = io.resetEnable;
    data.bit(8, 14) = 0;
    data.bit(15) = vdp.framebufferAccess;
  }

  //interrupt control
  if(address == 0xa15102) {
    data.bit(0)     = shm.irq.cmd.active;
    data.bit(1)     = shs.irq.cmd.active;
    data.bit(2, 15) = 0;
  }

  //bank set
  if(address == 0xa15104) {
    data.bit(0,1) = io.romBank;
    data.bit(2, 15) = 0;
  }

  //data request control
  if(address == 0xa15106) {
    data.bit(0) = dreq.vram;
    data.bit(1) = dreq.dma;
    data.bit(2) = dreq.active;
    data.bit(3,6) = 0;
    data.bit(7) = dreq.fifo.full();
    data.bit(8,15) = 0;
  }

  //68K to SH2 DREQ source address
  if(address == 0xa15108) {
    data.byte(0) = dreq.source.byte(2);
  }
  if(address == 0xa1510a) {
    data.byte(1) = dreq.source.byte(1);
    data.byte(0) = dreq.source.byte(0);
  }

  //68K to SH2 DREQ target address
  if(address == 0xa1510c) {
    data.byte(0) = dreq.target.byte(2);
  }
  if(address == 0xa1510e) {
    data.byte(1) = dreq.target.byte(1);
    data.byte(0) = dreq.target.byte(0);
  }

  //68K to SH2 DREQ length
  if(address == 0xa15110) {
    data.byte(1) = dreq.length.byte(1);
    data.byte(0) = dreq.length.byte(0);
  }

  if(address == 0xa15114 ||
     address == 0xa15116 ||
     address == 0xa15118 ||
     address == 0xa1511c ||
     address == 0xa1511e) {
    data = 0;
  }

  //TV register
  if(address == 0xa1511a) {
    data.bit(0) = io.cartridgeMode;
    data.bit(1,15) = 0;
  }

  //communication
  if(address >= 0xa15120 && address <= 0xa1512f) {
    data = communication[address >> 1 & 7];
  }

  //PWM control
  if(address == 0xa15130) {
    data.bit(0,1)  = pwm.lmode;
    data.bit(2,3)  = pwm.rmode;
    data.bit(4)    = pwm.mono;
    data.bit(7)    = pwm.dreqIRQ;
    data.bit(8,11) = pwm.timer;
  }

  //PWM cycle
  if(address == 0xa15132) {
    data.bit(0,11) = pwm.cycle;
  }

  //PWM left channel pulse width
  if(address == 0xa15134) {
    data.bit(0,12) = pwm.lfifoLatch;
    data.bit(14)   = pwm.lfifo.empty();
    data.bit(15)   = pwm.lfifo.full();
  }

  //PWM right channel pulse width
  if(address == 0xa15136) {
    data.bit(0,12) = pwm.rfifoLatch;
    data.bit(14)   = pwm.rfifo.empty();
    data.bit(15)   = pwm.rfifo.full();
  }

  //PWM mono pulse width
  if(address == 0xa15138) {
    data.bit(0,12) = pwm.mfifoLatch;
    data.bit(14)   = pwm.lfifo.empty() && pwm.rfifo.empty();
    data.bit(15)   = pwm.lfifo.full()  || pwm.rfifo.full();
  }

  //bitmap mode
  if(address == 0xa15180) {
    data.bit( 0) = vdp.mode.bit(0);
    data.bit( 1) = vdp.mode.bit(1);
    data.bit( 6) = vdp.lines;
    data.bit( 7) = vdp.priority;
    data.bit(15) = !Region::PAL();
  }

  //packed pixel control
  if(address == 0xa15182) {
    data.bit(0) = vdp.dotshift;
  }

  //autofill length
  if(address == 0xa15184) {
    data.byte(0) = vdp.autofillLength;
  }

  //autofill address
  if(address == 0xa15186) {
    data = vdp.autofillAddress;
  }

  //autofill data
  if(address == 0xa15188) {
    data = vdp.autofillData;
  }

  //frame buffer control
  if(address == 0xa1518a) {
    data.bit( 0) = vdp.framebufferActive;
    data.bit( 1) = MegaDrive::vdp.refreshing()
                || vdp.framebufferEngaged(); // FEN: frame buffer engaged
    data.bit( 2, 12) = 0;
    data.bit(13) = !vdp.paletteEngaged();    // PEN: can access palette
    data.bit(14) = vdp.hblank;
    data.bit(15) = vdp.vblank;
  }

  //palette
  if(address >= 0xa15200 && address <= 0xa153ff) {
    if(vdp.framebufferAccess) return data;
    while(vdp.paletteEngaged()) {
      if(cpu.active()) cpu.wait(1);
    }
    data = vdp.cram[address >> 1 & 0xff];
  }

  return data;
}

auto M32X::writeExternalIO(n1 upper, n1 lower, n24 address, n16 data) -> void {
//print("w", hex(address), " = ", hex(data), "\n");

  //adapter control
  if(address == 0xa15100) {
    if(lower) {
      io.adapterEnable = data.bit(0);

      if(data.bit(1) == 0) {
        shm.restart();
        shs.restart();
      }

      io.adapterReset = data.bit(1);
    }
    if(upper) {
      vdp.framebufferAccess = data.bit(15);
    }
  }

  //interrupt control
  if(address == 0xa15102) {
    if(lower) {
      shm.irq.cmd.active = data.bit(0);
      shs.irq.cmd.active = data.bit(1);
    }
  }

  //bank set
  if(address == 0xa15104) {
    if(lower) {
      io.romBank = data.bit(0,1);
    }
  }

  //data request control
  if(address == 0xa15106) {
    if(lower) {
      dreq.vram   = data.bit(0);
      dreq.dma    = data.bit(1);
      if(dreq.active > data.bit(2)) {
        // Night Trap 32X: delay between the last fifo write and flush on disable
        if(cpu.active()) cpu.wait(1);
        // Note: The current fifo implementation is incorrect (hw uses dual-block fifo)
        // but dreq dma works fine as is for normal cases.

        dreq.fifo.flush();
        shm.dmac.dreq[0] = 0;
        shs.dmac.dreq[0] = 0;
      }
      dreq.active = data.bit(2);
    }
  }

  //68K to SH2 DREQ source address
  if(address == 0xa15108) {
    if(lower) dreq.source.byte(2) = data.byte(0);
  }
  if(address == 0xa1510a) {
    if(upper) dreq.source.byte(1) = data.byte(1);
    if(lower) dreq.source.byte(0) = data.byte(0);
  }

  //68K to SH2 DREQ target address
  if(address == 0xa1510c) {
    if(lower) dreq.target.byte(2) = data.byte(0);
  }
  if(address == 0xa1510e) {
    if(upper) dreq.target.byte(1) = data.byte(1);
    if(lower) dreq.target.byte(0) = data.byte(0);
  }

  //68K to SH2 DREQ length
  if(address == 0xa15110) {
    if(upper) dreq.length.byte(1) = data.byte(1);
    if(lower) dreq.length.byte(0) = data.byte(0) & ~3;
  }

  //FIFO
  if(address == 0xa15112) {
    if(!lower || !upper) debug(unusual, "[DREQ] fifo byte write ", lower ? "(odd)" : "(even)");
    if(dreq.active && !dreq.fifo.full()) {
      dreq.fifo.write(data);
      if(!--dreq.length) dreq.active = 0;
      shm.dmac.dreq[0] = !dreq.fifo.empty();
      shs.dmac.dreq[0] = !dreq.fifo.empty();
    } else {
      debug(unusual, "[DREQ] missed fifo write");
    }
  }

  //TV register
  if(address == 0xa1511a) {
    if(lower) {
      io.cartridgeMode = data.bit(0);
    }
  }

  //communication
  if(address >= 0xa15120 && address <= 0xa1512f) {
    if(lower) communication[address >> 1 & 7].byte(0) = data.byte(0);
    if(upper) communication[address >> 1 & 7].byte(1) = data.byte(1);
  }

  //PWM control
  if(address == 0xa15130) {
    if(lower) {
      pwm.lmode   = data.bit(0,1);
      pwm.rmode   = data.bit(2,3);
      pwm.mono    = data.bit(4);
    //pwm.dreqIRQ = data.bit(7) = readonly;
    }
    if(upper) {
    //pwm.timer = data.bit(8,11) = readonly;
    //pwm.periods = 0 = readonly;
    }
  }

  //PWM cycle
  if(address == 0xa15132) {
    if(lower) pwm.cycle.bit(0, 7) = data.bit(0, 7);
    if(upper) pwm.cycle.bit(8,11) = data.bit(8,11);
  }

  //PWM left channel pulse width
  if(address == 0xa15134) {
    if(upper) pwm.lfifoLatch.bit(8,11) = data.bit(8,11);
    if(lower) {
      pwm.lfifoLatch.byte(0) = data.byte(0);
      pwm.lfifo.write(pwm.lfifoLatch);
    }
  }

  //PWM right channel pulse width
  if(address == 0xa15136) {
    if(upper) pwm.rfifoLatch.bit(8,11) = data.bit(8,11);
    if(lower) {
      pwm.rfifoLatch.byte(0) = data.byte(0);
      pwm.rfifo.write(pwm.rfifoLatch);
    }
  }

  //PWM mono pulse width
  if(address == 0xa15138) {
    if(upper) pwm.mfifoLatch.bit(8,11) = data.bit(8,11);
    if(lower) {
      pwm.mfifoLatch.byte(0) = data.byte(0);
      pwm.lfifo.write(pwm.mfifoLatch);
      pwm.rfifo.write(pwm.mfifoLatch);
    }
  }

  //bitmap mode
  if(address == 0xa15180) {
    if (vdp.framebufferAccess) return;
    if(lower) {
      vdp.mode     = data.bit(0,1);
      vdp.lines    = data.bit(6);
      vdp.priority = data.bit(7);
    }
  }

  //packed pixel control
  if(address == 0xa15182) {
    if (vdp.framebufferAccess) return;
    if(lower) {
      vdp.dotshift = data.bit(0);
    }
  }

  //autofill length
  if(address == 0xa15184) {
    if (vdp.framebufferAccess) return;
    if(lower) {
      vdp.autofillLength = data.byte(0);
    }
  }

  //autofill address
  if(address == 0xa15186) {
    if (vdp.framebufferAccess) return;
    if(upper) vdp.autofillAddress.byte(1) = data.byte(1);
    if(lower) vdp.autofillAddress.byte(0) = data.byte(0);
  }

  //autofill data
  if(address == 0xa15188) {
    if (vdp.framebufferAccess) return;
    if(upper) vdp.autofillData.byte(1) = data.byte(1);
    if(lower) vdp.autofillData.byte(0) = data.byte(0);
    vdp.fill();
  }

  //frame buffer control
  if(address == 0xa1518a) {
    if (vdp.framebufferAccess) return;
    if(lower) {
      vdp.selectFramebuffer(data.bit(0));
    }
  }

  //palette
  if(address >= 0xa15200 && address <= 0xa153ff) {
    if(vdp.framebufferAccess) return;
    while(vdp.paletteEngaged()) {
      if(cpu.active()) cpu.wait(1);;
    }
    if(upper) vdp.cram[address >> 1 & 0xff].byte(1) = data.byte(1);
    if(lower) vdp.cram[address >> 1 & 0xff].byte(0) = data.byte(0);
  }
}
