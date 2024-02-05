
# Geometry FOSS

This is an Open Source clone of Geomety Dash written in C using [raylib](https://www.raylib.com/).

## Building

There aren't any releases yet, so you have to compile GDF yourself for now.

### Linux

First, bootstrap nob. You only have to do this once.

```console
$ gcc -o nob nob.c
```

Then, you can build and run Geometry FOSS by running these commands:

```console
$ ./nob build
$ ./build/geometryfoss
```

If you want to cross compile for Windows, install mingw and run

```console
$ ./nob build win32-mingw
```

### Windows

First, install [mingw](https://github.com/niXman/mingw-builds-binaries/releases).

Then, bootstrap nob:
```powershell
gcc -o nob nob.c
```

Finally, compile Geometry FOSS using this command:
```powershell
./nob.exe build
```

There will be a `geometryfoss.exe` executable in the `build` directory.

## Disclaimer

Geometry Dash is made by RobTop Games.
I'm going to use the original assets, but in a way that is respectful (I hope). Please refer to [the plan](THEPLAN.md) for information on how I'm going to do that.
