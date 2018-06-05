# **Operators**
Operators are like two parameter functions, where the first parameter (caller) is passed before the function name.
They can be defined for any table by naming a block based on this table:

| Definition name | Operator |
| --------------- | -------- |
| __add | + |
| __sub | \- |
| __mul | * |
| __div | / |
| __mod | % |
| __pow | ^ |
| __eq | == |
| __neq | != |
| __gr | > |
| __ls | < |
| __geq | >= |
| __leq | <= |
| __ran | .. |
| __and | and |
| __or | or |
| __xor | xor |
| __lsh | lsh |
| __rsh | rsh |

```oca
Vector2 = (
  x: 0,
  y: 0,
  __add: do with other
    return (x: self.x + other.x, y: self.y + other.y)
)
```
