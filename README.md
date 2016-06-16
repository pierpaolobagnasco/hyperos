# hyperos
Simple Operating System for microcontrollers.

The idea is to develop a very (relatively) simple OS with a very easy-to-read code. For these reasons, the code may not be 100% optimized (yet, but it may be in the future). Nevertheless, this allows students or non-experienced users to get an idea behind OS mechanisms.

This is nor a real-time OS, nor a secure OS.

Currently, only STM32F407 and STM32F429I Discovery boards are supported (both include an ARM-Cortex-M4F).

__Any contribution will be highly appreciated!__


## Features
- Create and remove tasks at runtime;
- Assign a different protected memory region to each task (it protects both the instructions and the data)<sup>[1](#myfootnote1)</sup>;
- ...


## Not supported
- Floating Point Unit (FPU) for Cortex-M4F/M7F.


## Known issues
*To be filled :)*

<br />
<hr />


#### *Footnotes*
<a name="myfootnote1">1</a>: When memory protection is used, some of the regions are reserved for the OS.
