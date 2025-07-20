# p-xing

> :warning: **Note:** This project is currently in an early draft stage. Use with caution and expect frequent updates.

Pixel Crossing Game (nonogram)

In its current state, P-XING is a simple program designed to process Plain PBM (Portable Bitmap) image files and analyze the pixel data to identify patterns of consecutive pixels.

The goal is to generate P-Xing grids from PBM files and construct PBM files from submitted frequency vectors (nonogram solver).

## Features

- Reads Plain PBM image files.
- Displays the image data.
- Analyzes the pixel data to identify consecutive pixel patterns.

## How to Build

To build P-XING from source, follow these steps:

1. Clone the repository (and its unity submodule for unit tests):
   ```
   git clone --recurse-submodules https://github.com/krouis/p-xing.git
   ```

2. Navigate to the project directory:
   ```
   cd p-xing
   ```

3. Create a build directory and navigate to it:
> :warning: **Note:** If you forgot to initialize the Unity test submodule you need to run `git submodule update --init --recursive` before building the project.
   ```
   mkdir build
   cd build
   ```

4. Generate build files using CMake:
   ```
   cmake ..
   ```

5. Build the project:
   ```
   make
   ```

6. Upon successful compilation, the executable will be generated in the `build/src` directory.

## Usage

After building P-XING, you can run it from the command line. Here's the basic usage:
```
./p-xing [OPTIONS] <PBM_FILE>
```

Options:
- `-h`: Display usage information.
- `-v`: Display version information.

Example:
```
./src/p-xing ../examples/p-xing.pbm
------------------------------------------------------------------------
 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 0 1 1 1 0 0 0 0 0 0 0 0 0 1 0 0 1 0 0 0 1 1 0 0 0 1 0 0 1 0 0 0 1 1 1 0
 0 1 0 0 1 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0 1 1 0 1 0 0 1 0 0 0 0
 0 1 1 1 0 0 0 1 1 1 1 0 0 0 1 1 0 0 0 0 1 1 0 0 0 1 0 1 1 0 0 1 0 1 1 0
 0 1 0 0 0 0 0 0 0 0 0 0 0 1 0 1 0 0 0 0 1 1 0 0 0 1 0 0 1 0 0 1 0 0 1 0
 0 1 0 0 0 0 0 0 0 0 0 0 0 1 0 0 1 0 0 0 1 1 0 0 0 1 0 0 1 0 0 0 1 1 1 0
 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
------------------------------------------------------------------------
print p-xing lines v1:
0
3 1 1 2 1 1 3
1 1 1 1 2 1 1
3 4 2 2 1 2 1 2
1 1 1 2 1 1 1 1
1 1 1 2 1 1 3
0
```

## Example PBM File

An example PBM file (`p-xing.pbm`) is provided in the `examples` directory. You can use this file to test P-XING.

## Running Unit Tests

After building, run unit tests using:
```
cd build
ctest
```

Alternatively, you can run the test binary directly:
```
./tests/pxing_tests
```

## License

This project is licensed under the BSD 2-Clause License. See the LICENSE file for details.

