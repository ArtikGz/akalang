# Aka Lang
The version 2 of the akali-lang, another project for learning about compiler development.<br/>
Only avaliable for x64 Linux.

## Build
Building the compiler:
```bash
$ git clone https://github.com/akalirep/akalang
$ cd akalang
$ g++ -o main *.cpp
```
Compiling a file:
```bash
$ ./main main.aka
```

## Examples
Hello world: 
```go
fnc main() > int {
  println("Hello, world");
}
```
