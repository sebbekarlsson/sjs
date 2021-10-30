<p align="center" style="text-align: center; width: 100%;">
    <img width="256px" src="img/logo.jpg"/>
</p>

# Super Javascript
> Javascript with superpowers

## What is it?
> It's a Javascript engine; currently just an interpreter but there are more features to come, including:
* compile to C
* compile to 32bit assembly
* compile to 64bit assembly
* bundle for web
* JSX support
* Typescript support

## How it's built
> Everything is implemented from the ground up here, meaning that the whole Javascript runtime
> is written from scratch.  
> We're talking lexical analysis, parsing, evaluation, and eventually also a frontend that will support `C`, `32bit assembly`, `64bit assembly`.

## Why?
> Well, I've always thought it would be cool to have a language simple as Javascript that could also be compiled to machine code. So that's one reason.  
> The _real_ reason is that I was writing my own web browser, and I was looking for a Javascript engine;
> So I thought it would be fun to write my own.  
> Eventually some other ideas came to mind as well and now it's not just a Javascript engine anymore (theoretically).

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

## TODO
- [ ] compile to C
- [ ] compile to 32bit assembly
- [ ] compile to 64bit assembly
- [ ] bundle for web
- [ ] JSX support
- [ ] Typescript support
