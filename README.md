# ttf2svg (create character SVG from Truetype)

### Command Line
  `ttf2svg font.ttf codepoint`

### Examples
  `ttf2svg kaiu.ttf 99AC > horse-traditional.svg` \
  `ttf2svg simsun.ttc 9a6c > horse-simplified.svg`  

### To Compile
For Debian 9: \
`ape install build-essential libfreetype6-dev`
  
For Debian 10: \
`ape install build-essential libfreetype6-dev pkg-config`

and change first two lines of Makefile as below \
`CFLAGS=$(shell pkg-config freetype2 --cflags) -g -Wall -std=gnu99 -O2` \
`LIBS=$(shell pkg-config freetype2 --libs) -lm`
