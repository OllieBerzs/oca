# Table
Functions usable on any table.
___
> table.size -> int

Returns the number of elements in a table.

Example:
```oca
table = (1, 2, 3)
table.size
#returns: 3
```
___
> table.insert ([int/str], [any]) -> table

Inserts a value at the specified key and returns the table.

Example:
```oca
table = (1, 2, 3)
table.insert (2, true)
#returns: (1, 2, true, 3)
```
___
> table.remove [int/str] -> table

Removes an element at the specified key and returns the table.

Example:
```oca
table = (1, 2, 3)
table.remove 1
#returns: (1, 3)
```
___
> table.at [int/str] -> any

Returns the element at the specified key.

Example:
```oca
table = (1, 2, 3)
table.at 0
#returns: 1
```
___
> table.each yield ([int/str], [any]) -> table

Loops through the table and executes the yield block for each element, returns table.

Example:
```oca
table = (1, 2, 3)
table.each do with index, value
  println "{value} at index {index}"
#outputs:
# 1 at index 0
# 2 at index 1
# 3 at index 2
```
___
> table.sort yield ([any], [any]) -> table

Sorts the indexed elements of the table based on the yield condition.

Example:
```oca
table = (1, 2, 3)
table.sort do with a, b
  a > b
#returns: (3, 2, 1)
```
___
