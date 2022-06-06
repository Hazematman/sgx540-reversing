This document describes the (reverse engineered portions of) glGetProgramBinaryOES's output (TODO: more readable format)
Note that the shader binaries here are not the actual GPU code, there's a secondary finalization step the driver does on them, which I have not reverse engineered yet, in general the dumps from this call do not seem to be of much use for now.

4 bytes:  magic number 0x38B4FA10
4 bytes:  hash (the type of the hash is unknown at the moment)
20 bytes: data that is seemingly constant accross shader compiles and Opengl ES versions
4 bytes:  size of the rest of the file
36 bytes: Vertex shader information (format unknown)
4 bytes: length of the vertex shader (`vert_len`)
`vert_len` bytes: vertex shader
1 byte: 0
4 bytes: size of the constant array (`n_const_v`)
`n_const_v` * 32-bit: values of constants used by the shader, singular floats are still stored as 4-component vectors
2 bytes: size of symbols (`n_syms_v`)
1 byte: 0
`n_syms_v` * { 0-terminated string, 19 bytes of info }
36 bytes: Fragment shader information (format unknown)
4 bytes: length of the fragment shader (`frag_len`)
`frag_len` bytes: fragment shader
4 bytes: length of the fragment shader 2 (`frag_len_2`)
`frag_len_2` bytes: code for another fragment shader, the output looks similar to the regular fragment shader with minor differences
4 bytes: size of the constant array (`n_const_f`)
`n_const_v` * 32-bit: values of the constants
2 bytes: size of symbols (`n_syms_f`)
1 byte: 0
`n_syms_f` * { 0-terminated string, 19 bytes of info }

shader format (very wip):
1 byte: unknown
"PSU"
4 bytes: probably revision information 
4 bytes: (little endian) length of the rest of the binary
4 bytes: 01 for all
4 bytes: 01 again
4 bytes: 0
4 bytes: sometimes 0x40 sometimes 00?
off: 0x24
05 00 00 00 78 00 00 00
4 bytes: 0
4 bytes: sometimes 8?

all shaders end in:
32 bits: 0x8
looks like a tag - data format
