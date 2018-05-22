# Real
Functions usable on any real number.
___
> oper real.\_\_add [int/real] -> real

Returns the sum of the two numbers.

Example:
```oca
5.5 + 3.3
#returns: 8.8
```
___
> oper real.\_\_sub [int/real] -> real

Returns the difference between the two numbers.

Example:
```oca
5.5 - 3.3
#returns: 2.2
```
___
> oper real.\_\_mul [int/real] -> real

Returns the multiplication of the two numbers.

Example:
```oca
5.5 * 2
#returns: 11.0
```
___
> oper real.\_\_div [int/real] -> real

Returns the division of the two numbers.

Example:
```oca
5.0 / 2
#returns: 2.5
```
___
> oper real.\_\_pow [int/real] -> real

Returns the exponent of the real to the given power.

Example:
```oca
2.0 ^ 2
#returns: 4.0
```
___
> oper real.\_\_gr [int/real] -> bool

Returns if the real is greater than the other number.

Example:
```oca
5.0 > 4.5
#returns: true
```
___
> oper real.\_\_ls [int/real] -> bool

Returns if the real is lesser than the other number.

Example:
```oca
5.0 < 3
#returns: false
```
___
> real.floor -> int

Returns the conversion from a real number to an integer rounded down.

Example:
```oca
65.6
#returns: 65
```
___
> real.ceil -> int

Returns the conversion from a real number to an integer rounded up.

Example:
```oca
65.2
#returns: 66
```
___
> real.round -> int

Returns the conversion from a real number to an integer rounded.

Example:
```oca
65.6
#returns: 66
```
___
