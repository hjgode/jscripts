This workspace contains libraries used for multiple of my programs.

MPImg.dll (project MPImg) contains methods to load and draw images

MortZip.dll (project unzip) contais methods for handling of Zip archives.
Despite the historical project name, this also includes packing.

ZLib.lib (project ZLib) is a compression library. It's used both for Zip
archives and PNG images.

MPBitmap.lib (project MPBitmap) is a small library that helps to use
MPImg.dll without needing it to stay statically linked and thus occupying
about 400kB if it isn't required anymore.
