# **Files**
The file expression is a '$' (dollar sign) followed by a filepath without the extension.
```oca
$directory/file
```

The expression will evaluate the file as a Oca language file and then return
the table made from the file scope.
```oca
# math.oca
pub factorial = do with n
  if n == 0 then 1
  else n * factorial n-1
```
