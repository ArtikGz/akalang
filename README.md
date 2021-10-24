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
```rust
fnc main() > int {
	println("Hello, world");
	return 0;
}
```

### Includes
test.aka:
```rust
fnc test() > int {
	println("Hello, world!");
	return 0;
}
```

main.aka:
```rust
include "test.aka";

fnc main() > int {
	test();
	return 0;
}
```

### Variables
```rust
fnc main() > int {
	var varname: str = "Hello, world";
	println(varname); // Hello, world
	varname = "Hello, world!!!";
	println(varname); // Hello, world!!!
	return 0;
}
```

### While
```rust
fnc main() > int {
	var i: int = 0;

	while i < 5 {
		printint(i);
		i = i + 1;
	}

	println("end");
}
```

### If
```rust
fnc test(num: int) > int {
	if num + 5 > 10 {
		println("Hello, world");
	} else {
		println("Bye, world");
	}

	return 0;
}

fnc main() > int {
	test(5); // output: Bye, world
	test(6); // output: Hello, world

	return 0;
}
```

### Fibonacci
```rust
fnc fib(n: int) > int {
	if n < 2 {
		return n;
	}

	var a: int = fib(n - 1);
	var b: int = fib(n - 2);
	return a + b;
}

fnc main() > int {
	printint(fib(7)); // output: 13

	return 0;
}
```

### Prime numbers
```rust
fnc isPrime(n: int) > bool {
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

fnc main() > int {
	var n: int = 7;
	var test: bool = isPrime(n);
	if test {
		printint(n);
		println("is prime");
	} else {
		printint(n);
		println("is not prime");
	}

	return 0;
}
```