# **Assignment**
A variable of any type can be assigned to a name like so:
```oca
integer = 5
string = 'apple'
# etc.
```

The 'pub' keyword can be added to make the variable public when accessed from
an outer scope or a differen

### **Splitting**
If there are multiple variable names supplied on the left side of the assignment
operator, then the variable (indexed table) will be split to fit the number of
variable names.
```oca
a, b = (3, 4)
# a = 3
# b = 4
```

### **Star assignment**
If the variable name on the left is an '\*' (asterisk), then the named table on
the right will be opened up and imported in the current scope.
```oca
* = (a: 2, b: 5)
# a = 2
# b = 5

# star assignment used to import an external file
* = $math
c = factorial 5 # 'factorial' imported with star assignment
```

Star assignment can also be used as class inheritance.
