# GRRMOD-XMP

GRRMOD forked to use libopenmpt

<!--[![Actions Status](https://github.com/GRRLIB/GRRMOD-XMP/workflows/Continuous%20Integration/badge.svg)](https://github.com/GRRLIB/GRRMOD-XMP/actions)-->

## Installing GRRMOD

GRRMOD-XMP can be installed using CMake.

For Wii:

```bash
/opt/devkitpro/portlibs/wii/bin/powerpc-eabi-cmake -B build
cmake --build build --target install
```

For Gamecube:

```bash
/opt/devkitpro/portlibs/gamecube/bin/powerpc-eabi-cmake -B build
cmake --build build --target install
```

This process may take some time depending on the speed of your PC.

## Using GRRMOD

After everything is installed, simply put

```c
#include <grrmod.h>
```

at the top of your .c/.cpp file and use the functions as required

You will also need to add

```make
-lgrrmod -lopenmpt -lvorbisfile -lvorbis -logg -lmpg123 -lstdc++
```

to the libs line in your makefile

...Remember the order of the libraries is critical.  You may (need to) insert
other libraries in the middle of the list, you may need to add others to the
start, or even the end - but do _not_ change the order of the libraries shown
here.

## Licence

See the [LICENCE](LICENCE.md) file for licence rights and limitations (MIT).
