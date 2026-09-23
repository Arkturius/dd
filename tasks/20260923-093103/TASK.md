# Observe blank spot behaviour

- STATUS: OPEN
- PRIORITY: 1
- TAGS: decoder

Some blanks of the opcode map are treated as real instructions
```bash
0xf6 ModR/M [xx 001 xxx] -> TEST Eb, Ib
0xf7 ModR/M [xx 001 xxx] -> TEST Ev, Iz
```
Those disassemble AND execute as if the reg field was 000.
Explore more blank spots.
