## Notes on the code generation by the shader compiler

These are not meant to be an exact specification of the ISA but more along the lines of a log of the order in which i found things

### Registers
There seem to be 11 different kinds of "registers", so far these have been identified

- r#  : temporary
- o#  : output
- pa# : primary attribute
- sa# : secondary attribute
- c#  : probably (float?) constant
- i#  : floating-point internal register

Immediates are written as #num

### Prelude
There seems to be a prelude to vertex shaders that's approximately 81 instructions in size, it doesn't vary with any shader change i've done so far
it seems to mostly involve writing to output register 0 from the primitive attribute register 0 and "emitst" repeated over and over, it's followed a register write
to pa3 and pa2 wiuth floating point values 1.0 and 0.0, at this point the actual shader code seems to have any effect.

### Shader code

There are no "simd instructions", instructions which can be repeated work accross n consecutive registers

For vertex shaders, after this 81 instruction prelude is followed by instructions to actually manipulate the vertex information

The input vertex info is stored in the `pa0-pa3` registers, one component each, vertex output happens in two stages, first it writes to `o0-o3`, still one component per register,
then `emitvtx.end.freep` is issued, which terminates the vertex shader.

Other notable information is that each instruction that modifies the output registers seems to have a `skipinv` flag set, but the purpose of it remains unknown at the moment,
also `fmad` is always used for adding, `gl_Position = vPosition + vec4(0.2, 0.873, 0.592, 0.987);` is translated to `fmad.skipinv.repeat4 o0, pa0, c52, sa10`, the exact way the multiplication is "suppressed" is not known for now.

#### Uniforms
Uniforms seem to be read via a `smlsi` instruction, it has 11 (?) arguments and their meanings are now known, but as far as i can see it can load at most 4 components in a single instruction
the components seem to be read to the "secondary attribute" (`sa`) registers

### Varyings

Varying output to the fragment shader 

if you select the second component the compiler will break the add into more than 1 instruction
actually well, in general
```
"  gl_Position = vPosition + vec4(0.2, 0.873, 0.592, 0.987) + a + b + s + v;"
"  o = (vPosition + vec4(0.2, 0.873, 0.592, 0.987) + a + b + s + v).x;"
```

->
```
  90: 0x0082100570001A0D        fmad.skipinv o0, pa0, c52, sa13
  91: 0x00A22001703F9A11        fmad.skipinv.repeat3 o1, i2, c52, sa17
  92: 0x2881100160800000        mov.skipinv o4, o0
  ```

the 4th output is the "o" varying (equivalent of output in newer glsl)

code if `.y` is used instead:
```
  90: 0x008A10077000DA10        fmad.skipinv i0, pa3, c52, sa16
  91: 0x0082100570001A11        fmad.skipinv o0, pa0, c52, sa17
  92: 0x00A21001703F9A0D        fmad.skipinv.repeat2 o1, i2, c52, sa13
  93: 0x00821001707F1A13        fmad.skipinv o3, i0, c52, sa19
  94: 0x2881100160800080        mov.skipinv o4, o1
```

other components vary the code in similar ways, it is at the moment unknown why it uses an i(nternal?) register

This indicates the existence of "repeat delay slots"

### Loops

This glsl code: `for (int i = 0; i < lc; i++) { gl_PointSize += 0.01; }` (and a passthrough `gl_Position`) gets translated to

```
mov.skipinv.repeat4 o0, pa0
fadd.skipinv.testpanz.chan0 !c0, p0, sa11, c48
!p0 br -#0x00000002
mov.skipinv pa1, c48
fmad.skipinv pa0, pa0, c52, sa10.flt16.0
fmad.skipinv pa1, pa1, c52, c52
fsub.skipinv.testnanz.chan0 !c0, p0, pa1, sa11
p0 br -#0x00000003
fmin.skipinv o4, sa10.flt16.2, pa0
emitvtx.end.freep #0 /* incp */, #0
```

The purpose of the first loop is at the moment unknown (miscompilation?)
If a loop with a uniform bound is not used then the compiler will "tile" the operation(s) with `repeat(n)`
