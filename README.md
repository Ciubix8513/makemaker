# Disclaimer
This code is shit and doesn't really work, i made this just when i was starting to try to understand makefile. So the makefile this program generates is trash.
I'm still leaving this opensource, just because one person's trash can be another's treasure 

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
