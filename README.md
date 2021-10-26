# bin28xp
I made this simple program because I was trying to write C programs for my TI-84 Plus graphing calculator. Using C to z80 compiler included in the [z88dk](https://github.com/z88dk/z88dk/wiki/Tool---zcc) I was able to generate the machine code but appmake tool that is included has some flaws when generating .8xp files, so I decided to make my own program to do it. I also am going to use this to document what I have learned about the file format in case anyone is curious because I couldn't find hardly anything online about it. You can find that in the wiki section of this repository.
# Usage
Using this command line tool is really easy. I put it in a directory that I added to my path so I could use it anywhere but that isn't necessary if you are okay with moving the file around or typing the whole path out every time.

This is the usage format:
```
  bin28xp [input file name]
```
It is really simple right now. You specify the input file name which has your unpackaged but compiled code and it will make a .8xp file with the same name, in the same directory and it tries to match the name that is shown in the calculator to your file name as well as it can.
