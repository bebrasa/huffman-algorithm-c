# Huffman Coding Project

This project implements Huffman coding for file compression and decompression. It includes functions to convert wide strings to ASCII strings, calculate character frequencies, create Huffman trees, and encode and compress strings.

## Files

- `code.h`: Header file containing the Huffman coding functions.
- `decode.h`: Header file for Huffman decoding functions (implementation not shown).
- `structs.h`: Header file containing the definitions of the structures used.

## Functions

### ToString
```c
unsigned char *ToString(wchar_t *str);
