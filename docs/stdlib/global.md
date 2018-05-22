# Global
Functions usable in any file or part of an Oca program.
___
> print [any] -> nil

Outputs the argument passed to the console.

Example:
```oca
print 'Hello, World!'
#outputs: Hello, World!
```
___

> println [any] -> nil

Outputs the argument passed to the console, followed by a newline.
___
> debug [any] -> nil

Outputs the argument passed to the console, formatted for debugging purposes.

Example:
```oca
debug 5
#outputs: 5 of type int at 0x1e5b70
```
___
> input -> str

Reads a line from the console.
___
> pause -> nil

Pauses the execution of the program until a key is typed.
___
> assert ([bool], [str]) -> nil

Errors with the given message if the expression is false.

Example:
```oca
assert (true == false, 'oops!')
#error with message 'oops!'
```
___
> error [str] -> nil

Errors with the given message.

Example:
```oca
error 'oops!'
#error with message 'oops!'
```
___
> type [any] -> str

Returns the type of the value.

Example:
```oca
type 10
#returns: 'int'
```
___
> abs [int/real] -> int/real

Returns the absolute value of a number.

Example:
```oca
abs (-2.3)
#returns: 2.3
```
___
> acos [real] -> real

Returns the arc cosinus of an angle in radians.

Example:
```oca
acos 0.0
#returns: 0.0
```
___
> asin [real] -> real

Returns the arc sinus of an angle in radians.
___
> atan [real] -> real

Returns the arc tangent of an angle in radians.
___
> acot [real] -> real

Returns the arc cotangent of an angle in radians.
___
> cos [real] -> real

Returns the cosinus of an angle in radians.
___
> sin [real] -> real

Returns the sinus of an angle in radians.
___
> tan [real] -> real

Returns the tangent of an angle in radians.
___
> cot [real] -> real

Returns the cotangent of an angle in radians.
___
> max ([int/real], [int/real]) -> int/real

Returns the maximum of the two numbers.

Example:
```oca
max (2, 5)
#returns: 5
```
___
> min ([int/real], [int/real]) -> int/real

Returns the minimum of the two numbers.
___
> rad [real] -> real

Returns the angle converted from degrees to radians.

Example:
```oca
rad 0.0
#returns: 0.0
```
___
> deg [real] -> real

Returns the angle converted from radians to degrees.
___
> pi -> real

Returns [pi](https://en.wikipedia.org/wiki/Pi).

Example:
```oca
pi
#returns: 3.141593
```
___
> log ([real], [real]) -> real

Returns the logorithm of the given base for the given number.

Example:
```oca
log (2.0, 8.0)
#returns: 3.0
```
___
> ln [real] -> real

Returns the logorithm of the base [e](https://en.wikipedia.org/wiki/E_(mathematical_constant)) for the given number.

Example:
```oca
ln 1.0
#returns: 0.0
```
___
> lg [real] -> real

Returns the logorithm of the base 10 for the given number.

Example:
```oca
lg 10.0
#returns: 1.0
```
___
> random -> real

Returns a pseudo-random number in the range [0;1].
___
> seed [int] -> nil

Seeds the pseudo-random number generator.
___
> sqrt [real] -> real

Returns the square root of the number.

Example:
```oca
sqrt 4.0
#returns: 2.0
```
___
> cbrt [real] -> real

Returns the cube root of the number.

Example:
```oca
cbrt 8.0
#returns: 2.0
```
___
> read [str] -> str

Returns the string read from the file with the given file path.

Example:
```oca
read 'alphabet.txt'
#returns: 'abcde..'
```
___
> write ([str], [str]) -> nil

Writes the given string to the given file.

Example:
```oca
write ('sentence', 'text.txt')
#writes 'sentence' to the file 'text.txt'
```
___
> date -> str

Returns the current date formatted (dd-mm-yy time).
___
> clock -> int

Returns the time the program has run in milliseconds.
___
> execute [str] -> nil

Executes the given string in the os specific console.
___
