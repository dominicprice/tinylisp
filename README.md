# tinylisp
C implementation of a minimal lisp dialect described [here](https://codegolf.stackexchange.com/questions/62886/tiny-lisp-tiny-interpreter?noredirect=1&lq=1)

## Build instructions
```
git clone https://github.com/dominicprice/tinylisp
cd tinylisp
mkdir build
cd build
cmake ..
make
```

## Running
The interpreter should start by running the `tinylisp` executable in the build directory. A list of commands can be seen by passing `--help` (at the time of writing this
is horribly incomplete, refer to the reference link above). It also accepts the 
optional switch `--nobanner` which prevents a startup copyright banner from being displayed.
You may also specify a filename which it will accept as a script file, and evaluate each expression in the file displaying the results.

## Example programs
Some example programs can be found in the `tests` directory. 

### Extended arithmetic operators
The builtins only support subtraction, but addition and multiplication can be implemented as follows
```
(d add
	(q (
		(a b)
		(s a (s 0 b))
	))
)

(d mul
	(q (
		(a b)
		(i b (add a (mul a (s b 1))) 0)
	))
)
```
and then
```
> (add 9 4)
13
> (mul 9 43)
387
```
Of course this implementation of multiplication will fail for negative inputs of `b`, but can be fixed as follows
```
(d mul*
	(q (
		(a b)
		(i (l b 0) (mul (s 0 a) (s 0 b)) (mul a b))
	))
)
```
