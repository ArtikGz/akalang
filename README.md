# Aka Lang
This project is for learning about compiler development.<br/>
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
```js
include "std/stdio.aka";

function main() -> int {
	puts("Hello, world\n");
	return 0;
}
```

### Includes
test.aka:
```js
include "std/stdio.aka";

function test() -> int {
	puts("Hello, world!");
	return 0;
}
```

main.aka:
```js
include "test.aka";

function main() -> int {
	test();
	return 0;
}
```

### Variables
```js
include "std/stdio.aka";

function main() -> int {
	var varname: *char = "Hello, world";
	puts(varname); puts("\n"); // Hello, world
	varname = "Hello, world!!!";
	puts(varname); puts("\n"); // Hello, world!!!
	return 0;
}
```

### While
```js
include "std/stdio.aka";

function main() -> int {
	var i: int = 0;

	while i < 5 {
		printint(i); puts("\n");
		i = i + 1;
	}

	return 0;
}
```

### If
```js
include "std/stdio.aka";

function test(num: int) -> int {
	if num + 5 > 10 {
		puts("Hello, world\n");
	} else {
		puts("Bye, world\n");
	}

	return 0;
}

function main() > int {
	test(5); // output: Bye, world
	test(6); // output: Hello, world

	return 0;
}
```

### Fibonacci
```js
include "std/stdio.aka";

function fib(n: int) -> int {
	if n < 2 {
		return n;
	}

	var a: int = fib(n - 1);
	var b: int = fib(n - 2);
	return a + b;
}

function main() -> int {
	printint(fib(7)); puts("\n"); // output: 13

	return 0;
}
```

### Prime numbers
```js
include "std/stdio.aka";

function isPrime(n: int) -> bool {
	if n == 1 {
		return false;
	}
	if n == 2 {
		return false;
	}

	var counter: int = 0;
	var i: int = 1;
	while i < n {
		if n % i == 0 {
			counter = counter + 1;
		}

		i = i + 1;
	}

	return counter < 2;
}

function main() -> int {
	var n: int = 101;
	printint(n);
	if isPrime(n) {
		puts(" is prime\n");
	} else {
		puts(" is not prime\n");
	}

	return 0;
}
```
