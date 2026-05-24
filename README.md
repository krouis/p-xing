# p-xing

A terminal-based nonogram (Picross) game. Load any Plain PBM (P1) image and play it as a puzzle — the clues are derived automatically from the pixel data.

## Features

- Derives row and column clues from any Plain PBM (P1) file
- Full ncurses TUI: clue display, cursor, cell fill/cross toggle
- Crosshair highlight on the active row and column
- Auto-cross: when a line's filled cells satisfy its clue, remaining unknowns are marked automatically
- Undo (`u`) with a 16-step history
- Elapsed timer, frozen at solve time
- Assist mode (`-a`): wrong cells highlighted in red
- Win detection with solve-time banner
- Three bundled puzzles (5×5 cross, 5×5 arrow, 7×7 house)

## How to Build

```sh
# Clone with the Unity test submodule
git clone --recurse-submodules https://github.com/krouis/p-xing.git
cd p-xing

# If you forgot --recurse-submodules:
# git submodule update --init --recursive

cmake -B build .
cmake --build build
```

The binary is at `build/src/p-xing`.

## Usage

```
p-xing [OPTIONS] <PBM_FILE>
```

| Option | Description |
|--------|-------------|
| `-h`   | Display usage information |
| `-v`   | Display version information |
| `-a`   | Enable assist mode (wrong cells highlighted in red) |

### Bundled puzzles

```sh
build/src/p-xing puzzles/cross-5x5.pbm
build/src/p-xing puzzles/arrow-5x5.pbm
build/src/p-xing puzzles/house-7x7.pbm
build/src/p-xing -a puzzles/house-7x7.pbm   # with error highlighting
```

You can also load any PBM image you create:

```sh
build/src/p-xing examples/p-xing.pbm
```

### Controls

| Key | Action |
|-----|--------|
| Arrow keys | Move cursor |
| `Space` | Fill / unfill cell |
| `x` | Cross / uncross cell |
| `u` | Undo last fill or cross |
| `r` | Restart puzzle |
| `q` | Quit |

## Running Tests

```sh
cd build && ctest
# or run the binary directly (must be run from build/ for relative paths):
cd build && ./tests/test_pbm
```

## License

BSD 2-Clause License. See the LICENSE file for details.
