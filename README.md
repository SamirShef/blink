# Blink programming language
**Blink** is a small programming language, looks like **C**, **C++** and **Rust**

## Types:
1) **i8** (*aka signed char*)
2) **i16** (*aka signed short*)
3) **i32** (*aka signed int*)
4) **i64** (*aka signed long*)
5) **f32** (*aka float*)
6) **f64** (*aka double*)
7) **u8** (*aka unsigned char*)
8) **u16** (*aka unsigned short*)
9) **u32** (*aka unsigned int*)
10) **u64** (*aka unsigned long*)
11) **bool**
12) **pointer for all types**

## Variable declaration
```cpp
var test: i32 = 10 + 20;        // muttable
const test2: i32 = 10 - 20;     // immutable
```

## Comments
```cpp
// single-line comment

/*
multi-
line
comment
*/
```

## Function declaration
```cpp
func test() : i32 {
    /*...*/
    return 0;
}
```

## If statements
```cpp
func main() : i32 {
    var cond: i32 = 0;

    if (2 == 1) {
        printf("Yes!");
    }
    else if (cond) {
        printf("Not first!");
    }
    else {
        printf("Not all!");
    }
    return 0;
}
```

## For cycle
```cpp
func main() : i32 {
    for (i: i32 = 0; i < 10; i += 1) {
        if (i == 5) {
            break;
        }
        else if (i == 3) {
            continue;
        }
        printf("%d", i);
    }

    var a: i32 = 20;
    for (a = 1; a < 20; a *= 2) {
        printf("%d", a);
    }
    return 0;
}
```

## Download
### Install LLVM
Arch/Manjaro:
```bash
sudo pacman -S --needed llvm clang cmake ninja
```

Debian/Ubuntu:
```bash
sudo apt update
sudo apt install -y llvm-dev clang cmake build-essential ninja-build
```

Fedora:
```bash
sudo dnf install -y llvm-devel clang cmake gcc-c++ ninja-build
```

### For clone this repo to your PC:
```bash
git clone https://github.com/SamirShef/blink
cd blink
```

### Create a directory that will contain the assembly:
```bash
mkdir build
```

### Configure cmake and building:
```bash
cmake -B build -N Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR=<path/to/llvm>
cd build
ninja
```
where `<path/to/llvm>` replace to real path to llvm.

### Move `blinkc` to `/usr/bin`:
```bash
mv blinkc /usr/bin/blinkc
```

## Use
For test Blink compiler, use
```bash
blinkc <path/to/src>
```

where `<path/to/src>` replace to real path.

For to see more examples, see `examples/`