
# MegaSquirtSimulator

## Dedicated MS1 / Slowduino-HC08 firmware simulator

### What is this?

MegaSquirtSimulator is a fork of [uCsim](http://www.ucsim.hu) (a
multi-architecture micro-controller simulator, GPL) narrowed down to one
job: running real MegaSquirt MS1 and Slowduino-HC08 firmware images on a
simulated MC68HC908GP32, continuously, with no interactive debug session
required.

It loads a `.s19`/`.ihx` firmware image, runs it free-running, exposes the
simulated SCI (serial) peripheral as a raw TCP socket so tools like
TunerStudio can connect (bridged with `socat` to a real serial device if
needed), drives a configurable crank-trigger-wheel pulse generator on the
IRQ pin (RPM + missing-tooth pattern, e.g. `36-1`), and logs ignition/
injection output pin transitions (PTA/PTB) so firmware behavior can be
validated end-to-end without real hardware.

See `./ucsim_ms1 --help` for the simplified CLI, or `docs/` for the
underlying uCsim engine documentation this fork builds on.

Everything not needed for that one job — the other ~20 architectures
uCsim used to support, the interactive debug shell as the default
workflow — was removed or demoted for this fork. The interactive debug
shell binary (`ucsim_m68hc08`) is still built and usable directly for
troubleshooting.

### Compilation

    ./configure
    make ms1

This builds only the m68hc08 core and its GP32 peripherals
(`src/sims/m68hc08.src/`). Then run:

    ./ucsim_ms1 --firmware path/to/firmware.ihx --tcp-port 12000 --rpm 800 --pattern 36-1

### Upstream

This fork descends from uCsim by Daniel Drotos and contributors
(http://www.ucsim.hu, https://github.com/danieldrotos/ucsim). See git
history for the original multi-architecture README and full contributor
list.
