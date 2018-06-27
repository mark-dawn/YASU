# YASU

## What's this?
__Yet Another Saturn__ to __USB__ is a small pet project with 3 aims:
1. Connect a couple of Saturn controllers to my PC for retrogaming
2. Learn how the USB protocol works
3. Learn how to make a PCB and other hardware shenanigans

While I do not expect it to blow anyone's mind, I believe that the code and board turned out quite good
and are usable both in practice and for learning purposes.

## Hardware support
For now only the standard Saturn controller is supported, and has been tested with Japanese `HSS-0101` controllers.

However the code is structured so it should be easy to add support for other Saturn peripherals;
for sure, support for the _3D Controller_ is planned since I own one of those.

Support for the Saturn Mouse might be harder since the board should reconfigure itself as a pointer HID device at enumeration time.

## Note about contacts
The connection between board and the Saturn plug is handled quite rudimentary at the moment.
Flat naked metal pads on tabs that fit inside the cavity of the plug are present,
but cannot make direct contact with the pads in the plug.

For now the "intended" way is to solder extension metal pads to make contact,
but it is quite hard to solder those manually and the connection can act shaky.

In the future I plan to swap those out for flat pogo pins, or if no suitable part exists, use a proper 3D printed receptacle.

## Credits
This project would have not been as easy as it was without various resources from the Internets.
* Objective development for the excellent [v-usb library](https://github.com/obdev/v-usb)
* [Raphaël Assénat](http://www.raphnet.net/index_en), that has a similar [project](http://www.raphnet.net/electronique/saturn_usb/index_en.php) with excellent documentation
* https://gamesx.com/controldata/saturn.htm for a starting point on how to read the standard controller
* Any kind soul that has written small tutorials for ~~messy sintax~~ makefiles

## TODO
* Fix contact between board and Saturn Plug
* Add support for 3D pad
* Extend credits list, expecially for HID configuration examples
* Look into how licenses work since v-usb is under the GPL-2, and go around and slap it in the files that necessitate it
* Add better documentation to `Saturn.c` to clarify which bits are getting extracted without having to look at the header file

## License
All the repository is licensed under the GPL-3. See `LICENSE` for more info.

    Copyright (C) 2018  Marco Albanesi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
