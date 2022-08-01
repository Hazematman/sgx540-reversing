This is an up to date overview of the architecture, the information here may be partially inaccurate.

### Registers
There seem to be 11 different kinds of "registers", so far these have been identified

- r#  : temporary
- o#  : output
- pa# : primary attribute
- sa# : secondary attribute
- c#  : hardwired floating-point constants
- i#  : floating-point internal register
- g#  : global
special:
- pclink : pc link register
- drc#   : some sort of barrier for memory reads

Immediates are written as #num

Additionally there is a special "index register", the index register is written to by encoding a destination that is a cosntant, the index register can be used with the special index bank to index another bank (not all of them are supported for this and the mechanism hasn't been explored entirely).

### Repeat

Most instructions support a `repeat#` flag, allowing the instruction's operation to be repeated across adjacent registers, for instance `mov.repeat2 o0, r0` would move both `r0` and `r1` to `o0` and `o1` respectively, this mechanism can also be used via swizzles, in this case it acts similarly to repeat but can reorder the elements:

mov.xzy o0, r0
->
```
mov o0, r0
mov o2, r1
mov o1, r2
```

Based on the compiler output there may be some delay (unknown if variable based on the instruction) for registers to actually be written

### Predicates

Instructions may be prefixed with a predicate, there are at least 2 predicate register that may be negated, there's also a `Pn` register but its purpose is at the moment unknown, predicate registers are written to by certain instructions, so far I've been able to encode a floating-point subtraction that checks for a zero result successfully but there seems to be a lot of available conditions and a channel mechanism.

### Branches and calls

There are two means of control flow, `br` and `ba`, respectively relative and absolute branch with an immediate value (in instruction, not bytes), the limit for the jumps seems to be 4096 instructions, code also seems to use `.savelink` to write to the link register upon a branch to implement calls, analysis of the microkernel code so far has not revealed the presence of any actual call stack beyond this, but there may be some unknown mechanism to save it.

### Memory

Store instructions have a format that looks like `stad [r3,+#0], r5`, "a" seems to stand for "absolute", while "d" is doubleword, other addressing modes have not been explores just yet, the offsets from the first register is in units of the store size (much like C pointer arithmetic, for `stad` +1 actually means +4 bytes), loads are in a similar format `ldad.bpcache r4, [r3,#1], drc0`, the last argument seems to be something along the lines of a barrier register, which can later be waited on with `wdf` (this hasn't been entirely confirmed yet, but the usage of the instruction in generated code sheds some light)
