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
### Hello world
```go
fnc main() > int {
	println("Hello, world");
	return 0;
}
```

### Includes
test.aka:
```go
fnc test() > int {
	println("Hello, world!");
}
```

main.aka:
```go
include "test.aka";

fnc main() > int {
	test();
	return 0;
}
```

### Variables
```go
fnc main() > int {
	var varname: str = "Hello, world";
	println(varname); // Hello, world
	varname = "Hello, world!!!";
	println(varname); // Hello, world!!!
	return 0;
}
```