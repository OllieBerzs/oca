# **Types**
Oca language has 6 basic types: integer, real, boolean, string, table and block.

### **Integer**
Integer (int) is a 64-bit whole number. It can be represented in code in 3 different ways:
* decimal: `50`
* binary: `0b110010`
* hexadecimal: `0x32`

### **Real**
Real (real) is a 64-bit floating-point number. It can be represented in code in 2 different ways:
* decimal: `34.56`
* scientific: `1.2e19`

### **Boolean**
Boolean (bool) is a binary data type which can be in one of two states: `true` (1) or `false` (0).

### **String**
String (str) is a sequence of [ASCII](https://www.asciitable.com/) characters. There are two types of strings in the Oca language:
* simple string: `'apple'`
* formatted string: `"i like {adjective} apples.\n"`

The formatted string has two special features that the simple string does not have. Escaped characters with `\` will be interpreted and inserted back into the string based on this table:

| Escape Sequence | Interpretation |
| --------------- | -------------- |
| \a | bell sound |
| \b | backspace |
| \f | form feed |
| \n | newline |
| \r | carriage return |
| \t | tab |
| \v | vertical tab |
| \\ | '\' literal |
| \{ | '{' literal |

Also any Oca code placed in '{}' (curly braces) will be interpreted and then inserted into the string, for example:
```oca
name = 'Calvin'
print "My name is {name}." # will print 'My name is Calvin.'
```

### **Table**
Table is a collection of data which can be used as many different data structures in other languages like:
* lists: `(1, 2, 3)`
* maps: `(('a', 1), ('b', 2), ('c', 3))`
* classes (with inheritance): `(a: 1, b: 2, c: 3)`
* and hybrids between these

More on tables and their specific use cases in a separate chapter.

### **Block**
A block is a collection of Oca expressions which can be executed with or without parameters. Coming from other languages these could be thought of as functions or methods.
* Block without argument:
```oca
  do
    age = 32
    println "Age is {age}"
```
* Block with argument:
```oca
  do with num
    return num * num
```

More on blocks and their specific use cases in a separate chapter.
