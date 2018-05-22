# **Hello, Oca!**
This tutorial will get you started with your first 'Hello, World!' program in Oca.

## **Requirements**
* gcc v7.1.0 (for compilation)
* git

## **How to get Oca**
First things first, you need to clone the [Oca git repository](https://github.com/OllieBerzs/oca). Open up your terminal and enter the following command:
```bash
$ git clone https://github.com/OllieBerzs/oca.git
```

Next you will need to compile the Oca executable:
```bash
$ cd oca
$ make release
```

Now just put the executable somewhere in your system path and you are good to go.

## **Launching Oca**
You can run the Oca REPL by using the command:
```bash
$ oca
```

A REPL prompt will appear. For your 'Hello, World!' program type:
```oca
[] print 'Hello, Oca!'
```

If it outputs 'Hello, Oca!' to the console, **congratulations**!
___
You can also run the oca command with a filepath as an argument (the file extension is .oca):
```bash
$ oca hello.oca
```
```oca
# hello.oca
print 'Hello, Oca!'
```
