# Makemaker

A simple tool to automatically generate a Makefile.

## Usage

```Bash
 makemaker -i <file> -c <compiler> -f <code filetype> -H <header filetype> -o <output name> <compiler arguments>
```

## Default parameters

- -c = g++
- -f = cpp
- -H = h
- -o = output

## Example

```Bash
makemaker -i main.cpp -c g++ -f cpp -H h -o main
