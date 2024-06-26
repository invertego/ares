struct MSX2 : System {
  auto name() -> string override { return "MSX2"; }
  auto loadMultiple(vector<string> location) -> bool override;
  auto save(string location) -> bool override;
};

auto MSX2::loadMultiple(vector<string> location) -> bool {
  if(location.size() != 2) return false;

  auto bios = Pak::read(location[0]);
  if(!bios) return false;

  auto sub = Pak::read(location[1]);
  if(!sub) return false;

  this->location = locate();
  pak = new vfs::directory;
  pak->append("bios.rom", bios);
  pak->append("sub.rom", sub);

  pak->append("time.rtc", 52);
  if(auto fp = pak->write("time.rtc")) {
    for(auto address : range(fp->size())) fp->write(0xff);
  }
  Pak::load("time.rtc", ".rtc");
  return true;
}

auto MSX2::save(string location) -> bool {
  return true;
}
