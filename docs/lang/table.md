# **More on tables**
Tables are the array, map, class type in the Oca language.

## **Definition**
The table definition starts with an `(` (open bracket) and ends with an `)` (closed bracket)
and the table's elements are separated with a `,` (comma). Element definitions can be either
on the same line or indented on the next. If a table element isn't given a name, it will be
assigned an index starting at 0. Names to elements can be assigned by giving a name followed
by a `:` (colon) before the value.
```oca
# unnamed (indexed) table
list = (1, 2, 3, 4, 7)

# named table (class)
dog = (
  name: 'Rex',
  age: 5
)
```

## **Accessing**
The elements of a table can be accessed in 2 different ways:
* By using a `.` (dot): `dog.name` or `list.0`
* By using the [standard library function](stdlib/table) 'at': `dog.at 'name'` or `list.at 0`

## **Inheritance**
A neat trick you can do with the [star assignment](lang/assignment) is make base and child classes.
```oca
Animal = (
  age: 0,
  speed: 1  
)

# 'Dog' is a child class of 'Animal'
Dog = (
  *: Animal,
  name: 'Rex',
  breed: 'shephard'
)

# 'Dog' has all the elements that 'Animal' has
Dog.age = 5
```

## **Self**
The `self` keyword will access the table itself from within one of the table's
named blocks.
```oca
Human = (
  name: 'Peter',
  age: 54,
  talk: do
    print "My name is {self.name} and i am {self.age} years old."
)
```
