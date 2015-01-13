# holestone-tartan

An archive of Robert Bradford's mktartan program, compiled from its
scattered remains on the net. Mostly recovered from archive.org —
[https://web.archive.org/web/*/http://www.holestone.net/tartan/](https://web.archive.org/web/*/http://www.holestone.net/tartan/)

## Build

    cc -o mktartan mktartan.c

## Usage

    mktartan -c -s2 -x tdf/Mitchell.tdf

which creates `tdf/Mitchell.xpm`

Samples folder contains PNG files converted from xpm files using the
example setting above.

## Licence

From the source:
    
    /*
    # Builds XPM or BMP files from a TDF tartan definition file.
    # Copyright (c) 1998 Robert Bradford. All Rights Reserved.
    # 
    # This software may be freely used for any purpose whatsoever, 
    # provided the copyright message is left intact. 
    # 
    # Have fun.
    */
    
