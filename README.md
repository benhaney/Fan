This repository contains the code and schematic for making an internet-connected fan (or anything else really) with an ESP8266

## Why?

My air conditioning sucks and I got tired of getting up to turn the fan on or off, so I connected it to the internet so I could turn it on without getting out of bed.

## How does it work?

The hardware is just an ESP8266 with one of its GPIO pins used to switch a high voltage relay in line with the fan's power.

![Schematic for internet connected fan](https://git.benhaney.com/Ben/Fan/raw/master/schematic.png)

The software for the ESP8266 just exposes an API endpoint for turning any of its GPIO pins on or off. If it doesn't have WiFi credentials stored in its EEPROM, it becomes a wireless access point and lets you connect directly to it to set its credentials.

This is intended to work in tandem with code running on other systems that expose the actual user interface and then perform an operation using the embedded device's API. For example, I have my server proxying a subdomain to my fan on my local network, and then I have a keybind on my laptop that curls the subdomain with a query to toggle the in-use GPIO pin.

Because of how generic this is, you could really use it as an internet connected power switch for any arbitrary appliance (I've done something similar in the past with light switches and it worked great despite probably being a huge fire hazard).

## License

[Creative Commons 0](https://creativecommons.org/publicdomain/zero/1.0/)
