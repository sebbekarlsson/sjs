<p align="center" style="text-align: center; width: 100%;">
    <img width="256px" src="img/logo.jpg"/>
</p>

# Super Javascript
> Javascript with superpowers

----------------------

## What is it?
> It's a Javascript engine; currently just an interpreter but there are more features to come, including:
* compile to C
* compile to 32bit assembly (in progress)
* compile to 64bit assembly
* bundle for web
* JSX support
* Typescript support

----------------------

## How it's built
> Everything is implemented from the ground up here, meaning that the whole Javascript runtime
> is written from scratch.  
> We're talking lexical analysis, parsing, evaluation, and eventually also a frontend that will support `C`, `32bit assembly`, `64bit assembly`.
> 
> This also means that every built-in symbol you would find in Javascript is also implemented from scratch, such as:
* Math
* Math.cos
* ...
* Object
* console
* ... etc

----------------------

## Why?
> Well, I've always thought it would be cool to have a language simple as Javascript that could also be compiled to machine code. So that's one reason.  
> The _real_ reason is that I was writing my own web browser, and I was looking for a Javascript engine;
> So I thought it would be fun to write my own.  
> Eventually some other ideas came to mind as well and now it's not just a Javascript engine anymore (theoretically).

----------------------

## Benchmarks
> You can view benchmarks [here](benchmarks.md)  
> (_only the interpreter has benchmarks at the moment_)

----------------------

## Using it
### Interpreter
> Using the interpreter is simple:
``` bash
sjs <input_file>.js
```
### Compiler
> When using `sjs` to compile, you need to specify the emit output.  
> The available emit outputs are:

* asm32 (_in progress_)
* asm64 (_not implemented_)
* C99 (_not implemented_)
* js (_not implemented_)

> The command you would run to compile a Javascript file to 32 bit assembly would look like this:

``` bash
sjs <input_file>.js -e asm32
```

> This will produce a bunch of files:
* <input_file>.js.s (_Assembly instructions_)
* <input_file>.js.o (_Object file (From the linker)_)
* <input_file>.js.out (_The actual executable_)

> Replace `-e asm32` with whatever emit output you want.  

----------------------

## Building it
### Build dependencies
> To build `sjs` you will need:
* cmake
* gcc
### Build instructions
> After you've made sure you've got all the dependencies listed above, you can run:
``` bash
mkdir -p build && cd build && cmake .. && make
```
> You will now have a binary that can run Javascript.

----------------------

## TODO
- [ ] compile to C
- [ ] compile to 32bit assembly (in progress)
- [ ] compile to 64bit assembly
- [ ] bundle for web
- [ ] JSX support
- [ ] Typescript support
- [ ] write custom assembler
- [ ] write custom linker

----------------------

## Notes
> Please note that this is a work in progress, and there might be undefined behavior or things just might not
> work the way you would expect.
