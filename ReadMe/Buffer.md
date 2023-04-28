# Buffer Raw Design

_________________
[Buffer](../Buffer/Buffer.hpp)

## Basic Raw View

about itself raw view it looks like this:

```cpp
//all members must large than 0 , so don't need to use int16_t or something just like this
uint16_t length{ 1 }; // buffer length if length is 0 means raw is null
uint16_t iParams{ 0 }; // params count
uint16_t count_offset{ 0 };// count offset
uint16_t key_offset{ 0 };// key offset
uint16_t value_offset{ 0 };// value offset
uint16_t buffer_offset{ 0 };// buffer offset
//about the Buffer Object's Raw View 
90 5b 57 8d   c7 01 00 00   1d 00 00 00   00 00 00 00   │ ·[W············· │
01 00 00 00   00 00 00 00   05 00 00 00   00 00 00 00   │ ················ │
05 00 00 00   00 00 00 00   05 00 00 00   00 00 00 00   │ ················ │
//Raw View
90 5b 57 8d   c7 01: unsigned char *raw{}; (0x01c78d575b90) is the real location 
00 00 : 2 paddings fill by system (wasted)
1d 00 00 00   00 00 00 00 : uint16_t length{ 1 }; // buffer length if length is 0 means raw is null
01 00 00 00   00 00 00 00 : uint16_t iParams{ 0 }; // params count
05 00 00 00   00 00 00 00 : uint16_t count_offset{ 0 };// count offset
05 00 00 00   00 00 00 00 : uint16_t key_offset{ 0 };// key offset
05 00 00 00   00 00 00 00 : uint16_t value_offset{ 0 };// value offset
```

in this structure, still has lots of memory wasted, such as using uint16_t will waste 4bytes each params
also the const char* will has 2 00 padding

_________________

## Real raw view

```cpp
//buffer raw view it will be looked like this
modifed flag     params count  data section  buffer end
00               00 00 00 00   ....          00
```
