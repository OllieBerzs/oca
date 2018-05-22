# Integer
Functions usable on any integer.
___
> oper int.\_\_add [int/real] -> int/real

Returns the sum of the two numbers.

Example:
```oca
5 + 3
#returns: 8
```
___
> oper int.\_\_sub [int/real] -> int/real

Returns the difference between the two numbers.

Example:
```oca
5 - 3
#returns: 2
```
___
> oper int.\_\_mul [int/real] -> int/real

Returns the multiplication of the two numbers.

Example:
```oca
5 * 3
#returns: 15
```
___
> oper int.\_\_div [int/real] -> int/real

Returns the division of the two numbers.

Example:
```oca
5 / 2.5
#returns: 2
```
___
> oper int.\_\_mod [int] -> int

Returns the remainder of the division of the two numbers.

Example:
```oca
5 % 3
#returns: 2
```
___
> oper int.\_\_pow [int/real] -> int/real

Returns the exponent of the integer to the given power.

Example:
```oca
5 ^ 3
#returns: 125
```
___
> oper int.\_\_eq [int] -> bool

Returns if the two integers are the same.

Example:
```oca
5 == 3
#returns: false
```
___
> oper int.\_\_neq [int] -> bool

Returns if the two integers are not the same.

Example:
```oca
5 != 3
#returns: true
```
___
> oper int.\_\_gr [int/real] -> bool

Returns if the integer is greater than the other number.

Example:
```oca
5 > 4.5
#returns: true
```
___
> oper int.\_\_ls [int/real] -> bool

Returns if the integer is lesser than the other number.

Example:
```oca
5 < 3
#returns: false
```
___
> oper int.\_\_geq [int] -> bool

Returns if the integer is greater than or equal to the other integer.

Example:
```oca
5 >= 3
#returns: true
```
___
> oper int.\_\_leq [int] -> bool

Returns if the integer is lesser than or equal to the other integer.

Example:
```oca
5 <= 3
#returns: false
```
___
> oper int.\_\_ran [int] -> table

Returns a table whose values range from one integer to the other (including).

Example:
```oca
3 .. 5
#returns: (3, 4, 5)
```
___
> oper int.\_\_and [int] -> int

Returns the result of a [bitwise](https://en.wikipedia.org/wiki/Bitwise_operation) 'and' operation.

Example:
```oca
0b0101 and 0b0011
#returns: 1
```
___
> oper int.\_\_or [int] -> int

Returns the result of a [bitwise](https://en.wikipedia.org/wiki/Bitwise_operation) 'or' operation.

Example:
```oca
0b0000 or 0b0011
#returns: 3
```
___
> oper int.\_\_xor [int] -> int

Returns the result of a [bitwise](https://en.wikipedia.org/wiki/Bitwise_operation) 'xor' operation.

Example:
```oca
0b0001 xor 0b0011
#returns: 2
```
___
> oper int.\_\_lsh [int] -> int

Returns the result of a [bitwise](https://en.wikipedia.org/wiki/Bitwise_operation) 'left shift' operation.

Example:
```oca
0b0001 lsh 1
#returns: 2
```
___
> oper int.\_\_rsh [int] -> int

Returns the result of a [bitwise](https://en.wikipedia.org/wiki/Bitwise_operation) 'right shift' operation.

Example:
```oca
0b0100 rsh 1
#returns: 2
```
___
> int.times yield [int] -> nil

Loops the given number of times and executes the yield block with an iterator.

Example:
```oca
3.times do with i
  print i
#outputs: 012
```
___
> int.ascii -> str

Returns the conversion of the integer based on the [ASCII](https://www.asciitable.com/) table.

Example:
```oca
65.ascii
#returns: 'A'
```
___
> int.real -> real

Returns the conversion from an integer to a real number.

Example:
```oca
65.real
#returns: 65.0
```
___
