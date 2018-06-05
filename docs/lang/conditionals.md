# **Conditionals**
There currently exists only one conditional in the Oca language - the `if` conditional.

## **If**
The `if` conditional executed the associated expressions only if the conditional
expression is true. If an `else` branch is provided, then it is executed
if the conditional expression is false.
```oca
# one executable expression
if conditional then print 'conditional is true'
else print 'conditional is false'

# multiple executable expressions
if a > 5 then
  b = a * 5
  a = b * 5
```

The `if` conditional will return a value, so it can be used when assigning a value.
```oca
string = if a == 5 then 'five' else 'not five'
```
