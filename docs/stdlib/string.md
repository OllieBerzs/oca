# String
Functions usable on any string.
___
> oper str.\_\_add [any] -> str

Returns the string concatination of the two.

Example:
```oca
'Hello, ' + 'World!'
#returns: 'Hello, World!'
```
___
> oper str.\_\_mul [int] -> str

Returns the string repeated the given number of times.

Example:
```oca
'Ho! ' * 3
#returns: 'Ho! Ho! Ho! '
```
___
> oper str.\_\_eq [str] -> bool

Returns if the strings are equal.

Example:
```oca
'this' == 'this'
#returns: true
```
___
> oper str.\_\_neq [str] -> bool

Returns if the strings are not equal.

Example:
```oca
'this' != 'this'
#returns: false
```
___
> str.size -> int

Returns the number of characters in the string.

Example:
```oca
'apple'.size
#returns: 5
```
___
> str.upcase -> str

Returns the string coverted to upper case.

Example:
```oca
'apple'.upcase
#returns: 'APPLE'
```
___
> str.lowcase -> str

Returns the string coverted to lower case.

Example:
```oca
'aPPle'.lowcase
#returns: 'apple'
```
___
> str.int -> int

Returns the string coverted to an integer.

Example:
```oca
'105'.int
#returns: 105
```
___
> str.real -> real

Returns the string coverted to a real number.

Example:
```oca
'105.4'.real
#returns: 105.4
```
___
> str.ascii -> int

Returns the string coverted based on the decimal [ASCII](https://www.asciitable.com/) value.

Example:
```oca
'A'.ascii
#returns: 65
```
___
> str.find [str] -> int

Returns the index at which it found the first given [regex](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_03) sequence.

Example:
```oca
'sausage'.find 'sag'
#returns: 3
```
___
> str.replace ([str], [str]) -> str

Returns the string with substrings replaced based on the [regex](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap09.html#tag_09_03) sequence.

Example:
```oca
'sausage'.replace ('s', 'f')
#returns: 'faufage'
```
___
> str.at [int] -> str

Returns the character at the given index.

Example:
```oca
'sausage'.at 2
#returns: 'u'
```
___
> str.each yield ([int], [str]) -> str

Loops through the string executing the yield block with each index and character, returns string.

Example:
```oca
'ace'.each do with i, c
  println "{c} at index {i}"
#ouputs:
# a at index 0
# c at index 1
# e at index 2
```
___
> str.split [str] -> table

Returns an indexed table of strings from the string split by the delimiter.

Example:
```oca
'this is an apple'.split ' '
#returns: ('this', 'is', 'an', 'apple')
```
___
