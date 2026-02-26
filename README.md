# repr - Binary Representation Utility

**repr** is a small command-line utility for inspecting and reasoning about binary representations, memory layouts, and bit-level structure of values. Its primary purpose is to make it easier to compute masks, visualize bit patterns, and understand how values are actually laid out in memory.

The tool operates on fixed 64-bit buffers. Inputs larger than 64 bits are truncated/wrapped; smaller inputs are zero-extended. Output is always a full 64-bit representation unless explicitly formatted otherwise.

This was originally written to debug endianness mismatches on a motorola 88100 emulation program.

---

## Supported Input Formats

The first argument (`CNF`) is a two-character format specifier:

```
<parse><print>
```

### Parse Specifiers

* `c` - chars (up to 8)
* `d` - decimal integer
* `x` - hexadecimal integer
* `b` - binary integer
* `f` - floating-point (double precision)

### Print Specifiers

* `d` - decimal
* `x` - hexadecimal
* `b` - binary (bit-ordered, mathematical view)
* `n` - native memory representation
* `s` - swapped native representation

Examples of valid CNF values:

* `db` - parse decimal, print binary
* `xb` - parse hex, print binary
* `fn` - parse float, print native bits
* `fs` - parse float, print swapped bits

## Usage

```
repr [CNF] <N>
```

Example:

```
repr db 42
repr xb ff
repr fn 3.1415926
repr fs 3.1415926
repr cx 'hello! '
```

## Output Modes

### Binary (`b`)

Prints the full 64-bit buffer in mathematical bit order (bit 63 → bit 0), followed by a reference header indicating bit positions.

### Decimal (`d`)

Prints the value as a base-10 integer.

### Hexadecimal (`x`)

Prints the value in hexadecimal.

### Native (`n`)

Prints the exact bit pattern as read byte-by-byte in memory, reflecting the host CPU’s endianness and layout.

### Swapped (`s`)

Prints the same native layout after a full 64-bit byte swap. This is useful for comparing host layout against opposite-endian representations.

Native and swapped modes always print all 64 bits.

## Floating Point Notes

* Floating-point input (`f`) is parsed as a `double`
* The printed value is the **raw IEEE-754 bit pattern**
* Decimal output is not meaningful for floating-point inputs
* Intended output formats for `f` are `b`, `x`, `n`, or `s`

---

## Build

The project builds as a single binary.

### Requirements

* GCC or Clang
* GNU extensions enabled

### Build Command

```
make
```

## Author

Jaime Castro Enrique - castro.jaime [at] proton.me

## LICENSE

```text
Copyright (c) 2025 Jaime Castro Enrique 

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and 
to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of
the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
```
