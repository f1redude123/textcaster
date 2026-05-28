# textcaster

It's not optimized or stylish, but it's mine and I'm proud of it.  

WASD to move, EQ to rotate camera  
To enter menu, press backtick (`)  
To change menu options, press number key of index of menu option(starting at 0) to cycle through  

The spaces in the font file are also kinda broken rn, if u wanna fix that and know how go ahead  

If you know anything about C, you can change some of the predefined vars in the code for higher res/different output dimensions.

Have fun :3

Also if you want to make custom fonts heres the format:

```
Fonts are stored in 6x6 layers of 1's and 0's, like this:
001000 010100 100010 111110 100010 000000
     ^                             ^
     |                             |
     |                             More space at the bottom for separation of glyphs
     |
     Empty bit on right side to leave space between glyphs

Fonts range from UTF-8 32(space character) to UTF-8 255(tilde).
To add your font once it's done, just fork the repo, drop in your font as "2.txt"(or a step up from your highest font) and increment index 1 of the "opmax" variable at the head of the C file.
```
