# Zen-lang
Aims to build an elegant, multi-paradigm, statically typed and general-purpose programming language.  
旨在构建一门优雅的、多范式及静态类型的通用编程语言。



## Get Started with Zen

- Write a Zen program in any editor and save it as "hello.zen".
```
// hello.zen

fun main -> Console.writeln('Hello World!')
```
- To use the compiler, type the command below in Shell and enter.
```
zc hello.zen
```
- Run the generated executable file.
```
zvm hello.zef
```
- Now you have finished a simple program, if you'd like to try further, you can also write the program above in Zen assembly language. 
```
;hello.asm

section .data
    hw String 'Hello World!'
    
section .code
    mov ax, 8h
    mov bx, hw
    int 11h
    mov ax, 0h
    int 0h
```
- Save the file as "hello.asm" and use the command:
```
zasm hello.asm
zvm hello.zef
```


## Web Service Sample

With ZenX (Zen Extended Library, which is still in plan), we can do more productive work easily with Zen. Here's a short web service program that can run on Linux server. 
```
import zenx.Sql
import zenx.Web

fun main {
    try {
        Sql.connect('localhost', 'username', 'password')
    } catch (String e) {
        Console.writeln(e)
    }
    
    while (true) {
        Web.response('GET', '/', 'Welcome to Zen web service.\n')
        Web.response('GET', '/time', () -> Time.getFullTime())
        Web.response('GET', '/db', () -> Sql.execute('SELECT * from table'))
    }
}
```
