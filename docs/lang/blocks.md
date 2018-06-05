# **More on blocks**
Blocks are the method or function type in the Oca language.

### **Definition**
The block definition starts with the keyword `do` which signifies that the next expression
or the next indented block of expressions is the body of the block.
```oca
# one expression
do print 'this'

# indented block of expressions
do
  print 'this'
  print 'that'
```

There can also be parameter names provided to the block, which can be used as variables in
the body of the block. This can be done by adding the `with` keyword after `do`.
```oca
# block with parameters 'a' and 'b'
do with a, b
  print a * b
  print a + b
```

### **Execution**
The block can be executed or called by using the name of the variable to which the block is assigned.
```oca
greet = do
  print 'Hello!'

# calling a block
greet
```

If the block needs parameters to be called, a value of any type (except block) can be passed
to the call.
```oca
show = do with message
  print message

# calling a block with a parameter
show 'this is a message'
```

If there are multiple parameters to the block, a table of indexed values can be passed and it will be split
to fit the necessary number of parameters.
```oca
multiply = do with a, b
  a * b

# calling a block with multiple parameters
c = multiply (3, 4)
```

A block will return either the value from the last expression or the value provided to the `return` keyword.

### **Yield**
There is a `yield` keyword available in each block which will trigger the supplied block.
```oca
# 'yield' will call the supplied block
foo = do
  yield 5

# supplying the yield block (the 'yield' block must be passed after all parameters)
foo do with n
  print 'foo passed ' + n
```
