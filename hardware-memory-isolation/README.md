# Hardware Memory Isolation

## Table of Contents

## Prerequisites

## Introduction

Besides computing, anotherrole of the hardware is to provide isolation, mainly between the operating system and the user applications.
You have encountered (hopefully) the main protection mechanism that the hardware uses to ensure memory isolation: pages.
There are other less-known mechanisms for ensuring memory isolation, through which we will go during this session: segments, privilege rings, memory protection keys.
We will also dive into virtualization, focusing on the hardware-assisted one.

## Memory Isolation

### Pages and Segments

The system needs a way to enforce ownership and permissons on the memory zones.
For example, it needs to enforce that a certain memory zone cand only be read and executed, not written.
How can this be achieved?
The first answer that the CPU designers came up with is called **segmentation**.

#### Segmentation

Segmentation is the x86 CPU feature that allows assigning permissions and ownership to a certain memory zone, using segments.
Segments differ from pages through their size and their organization, as you will see next.
Today, the modern systems don't use segmentation anymore, when performing usual operations.
Segmentation is only required at the early stages of booting, when pages cannot be used.
But segmentation is still there, and it is tied to many other components of the system, so knowledge of how segmentation works is useful.
Let's start at the beginning of time, with something called the **Real mode**.

##### Real Mode Memory Addressing

At the beginning of time, x86 CPUs only had 16-bit registers.
This meant that the maximum memory size that could be used, with **flat memory addressing** model, was 64KB (2^16).
It was thought that this amount of memory is enough, until it wasn't enough.
Instead of expanding the registers, the CPU manufacturers came up with the **segment memory addressing** model.
This meant that each memory address, instead of using one register, used 2: a normal register and a segment register, each fitting 16 bits.
The way the addresses were calculated was the following:

```
addrress = SEGMENT_REGISTER * 0x10 + OFFSET_REGISTER
```

This led to a new memory maximum of almost 1MB, which was tought to be enough.
Notice that, at that time, a segment was just a number.
It didn't enforce any permissions on the memory zone, or ownership, because there was no user-kernel separation.
The operating system had absolute power, and there was nothing but the operating system.
Then came 32-bit registers, the need for applications, the need for isolation.
The **protected mode** was born.

Note: real mode still exists.
Every x86 CPU starts in real mode, and must be switched to other modes, like the **Protected Mode**.

##### Protected Mode Memory Addressing

Well, all those things didn't appear at the same time.
First, the registers were extended to 32 bits.
Using the segmentation model, each segment would now fit 4GB of memory.
This meant that a system could use 68GB of memory.
But that memory wasn't available.
You could barely reach 4GB of RAM.
So, the addressing model was switched to the flat one: a memory address would be composed of only one register.

The segments got another role: enforce isolation (pages still weren't a thing).
A segment would not be a value used to compute an address, but an index in a table, the **GDT** (Global Descriptor Table).

###### The GDT

The GDT is a system table, that contains descriptors of memory zones: where they start, where they end, how they grow, whether they are readable, writeable, executable, who can access them.
Each entry in the GDT has 8 bytes in size, and it looks like this:

TODO: insert GDT entry diagram

Let's break each field down:
  * **Base**: the address at which the segment begins
  * **Limit**: the first wacky one; how many bytes or pages are contained in the segment
  * **Access**:
    TODO: insert Access Byte diagram  
    * **P**: Present - 1 if the segment is valid
    * **DPL**: Descriptor privilege level, where 0 is the highest privilege level, and 3 the lowest
    * **S**: Descriptor type; not interesting for us today
    * **E**: Executable; if 0, the segment is a data one; if 1, the segment is a code one
    * **D/C**: its significance depends on the **E** bit
      * if **E** is 0, the field is Direction; not interesting for today
      * if **E** is 1, the field is Conforming;
        * if the Conforming bit is 1, the code in this segment can be executed by equal or lower privilege level code;
          for example, code in a segment with **DPL** equal to 3, with **C** equal to 1, can be executed by code from a segment with **DPL** equal to 1
        * if the **C** bit is 0, the code in this segment can be executed only by code from segments with the same **DPL**
    * **R/W**: another field depending on **E**
      * if **E** is 0, the field is Writeable; data segments are always readable
      * if **E** is 1, the field is Readable; code semgments are never writeable
    * **A**: Accessed; not interesting
  * Flags:
    * **G** - Granularity: the **Limit** field is in bytes (0), or pages (1)?
    * **DB** - Size: if 1, the segment is a 32-bit one, else it is a 16-bit one
    * **L** - Long-mode code: if 1, the segment is a 64-bit code one
    TODO: insert Flags diagram

As complicated as it looks, The GDT did its job of enforcing some memory protection, hence a CPU with a GDT and 32-bit addresses is operating in the **Protected Mode**.
This came at the cost of the programmer's sanity.
If you are wondering what the designers of this model were smoking, you are not alone.

Fortunately, in modern systems, **Base** and **Limit** are ignored.
A segment always covers the entire address space.
Does that mean that the entire address space is in both the code and data segments?
Yes.
Then how do we ensure separation between executable and writeable memory zones?
Also, the entire memory is accesible by all privilege levels?
This doesn't seem right.
Enter pages.
But first, something about privilege levels, also known as **privilege rings**.

##### Privilege Rings

You should have heard about kernel-space and user-space.
How do we know if a memory zone belongs to the kernel-space, or the user-space?
That memory zone is part of a segment / a page, as we will see later, that belongs to either the kernel or the user.
The kernel-space is, in fact, any memory zone that belongs to **ring 0**, or DPL 0, and the user-space belongs to **ring 3**, or DPL 3.
What about the other rings, 1 and 2?
They can be used, but almost no one does it.
Some drivers use those rings, but it's not a common practice.

At this point, things get weird.
What if you want software with higher privileges than the kernel, like a hypervisor?
You get ring -1.
But what if you want a piece of code that is run by the hardware in critical moments?
You get ring -2.
Ring -3?
Someone got there.
Fear not, we will explore these weird notions later.
For now, let's do something practical.

##### Tutorial: Reading the GDT of the Linux Kernel

Go to the [`read-gdt`](./activities/read-gdt/) folder.
There you have a simple kernel module that reads the GDT of the operating system, then prints each field.
Run `make` to build the module, then `sudo insmod read_gdt.ko` to insert the module.
By running `sudo dmesg` you should see 16 GDT entries listed, the total size of the GDT and the virtual address where it is placed.
Only 16 entries are listed, because the ones after that are null.
Take a look at the entries, and figure out what entries 1 to 6 represent.
You should find 3 kernel entries, and 3 user ones.
Notice that entries 0 and 7 are null.
Entry 0 should always be null.
Entries from 8 onward are TSS and LDT entries, which won't be detailed in today's session.

Note that a special instruction, `sgdt` is used to retrieve the GDT pointer descriptor.
The opposite instruction is `lgdt`.

#### Paging

Soon enough, people got tired of dealing with segmentation;
a new method to divide the memory was needed.
Pages were born.
Unlike segments, that can be of any size, pages have fixed sizes: the standard one is 4KB, but it can also be of other sizes.
There are also the huge pages, that usually have 2MB, 4MB or 1GB.
Pages are organised hierarchically, in a tree-like structure.
A hardware component, called the MMU (Memory Management Unit) manages this structure.
We won't go into details about how that structure is organised, as to not transform this session into a Operating Systems design session.
What is important to know is that each page has permissions, that are checked by the MMU at every access.
The hardware doesn't, however, check if a memory page is accessed by the process that should be able to access it.
That is the role of the OS.

TODO: develop paging - at least MMU and TLB

#### Memory Protection Keys

Let's consider the following scenario:
an application wants to change an area of its memory from read-write to read-only, for reasons.
To do this it will call `mprotect()` on that area.
What will happen behind the scenes will be that the OS will change permissions for each page that is part of the memory area, then it will flush the TLB.
This is costly time-wise.
As a solution, Intel proposed the `MPK` set of instructions, that can quickly change permissions for an area of memory of any size.
How does this work?
Up to the moment when MPK was proposed, page-table entries had 4 bits that weren't used.
These 4 bits are tranformed into 16 possible `keys`.
Furthermore, a register, `PKRU`, is added to hold the permissions for each of those keys, local to each thread.
This allows an application to allocate its pages to a _protection domain_.
When accessing a page, instead of checking only the page permissions, the MMU will also check the protection domain permissions.

Let's take a practical example:
Application A has a page with read-write permissions.
It allocates a `protection domain` with read permissions, then adds the page to that protection domain.
When performing a write on that page, a Segmentation Fault will be received, because, even though the page has the right permissions, the protection domain does not.

Everything sounds nice, doesn't it?
Well, it is not.
The reason for this is that the instruction used to modify `PKRU` is unprivileged.
So, if an attacker gains the ability to execute arbitrary code, the whole mechanism can be bypassed.
Another problem is, as detailed by [this paper](https://arxiv.org/pdf/1811.07276v1.pdf), the fact that, after an application frees a protection domain, the key isn't deleted from the page-table entries.
So, if the same key is allocated again, it will still cover the previous pages, that should no longer be under a protection domain.
A classic example of `use-after-free`.
The final problem is that there are only 16 possible keys.
For the whole system.
A system that can run hundreds, if not thousands of processes, with many more threads.
You can see how this can go wrong.

##### Tutorial: MPK Basics

TODO: code where the students use `pkey_mprotect`

##### Activity: I Do This For Your Own Good

TODO: one program tries to read from a non-readable zone, enforced through PKU. The students must write a program to change the permissions of the `pkey`

### Control-Flow Enforcement

Don't you hate it when someone exploits your binary, using methods like `Return Oriented Programming` (ROP)?
(By the way, we have 2 sessions dedicated to ROP, [here]() and [here]()).
Well, other people hate it too, so they searched for a solution.
Intel's solution was adding control-flow enforcement in hardware, with the new `Control-Flow Enforcement Technology` instructions.
Those instructions are split in 2 parts:
* `Indirect Branch Tracking`, which checks if a `jmp` or `call` instruction targets a valid code address, marked accordingly by the programmer / compiler.
* `Shadow Stack`, which checks if the return address was altered in any way.

But wait, don't we have the `Stack Canary` for the last one?
The main problem with that solution is that the canary is placed on the same memory zone as the return address.
If we can modify the return address, what is stopping someone from reading the canary first, then modifying the return address, while keeping the canary intact?
The shadow stack places a copy of the return address in a separate memory zone, that can be accessed using only some special instructions.

#### Indirect Branch Tracking

The main idea behind some attacks is to chain small pieces of code (gadgets), in order to call a system function, usually, in a certain way.
Those gadgets aren't real functions that should be normally called, but rather pieces of a normal function, that end with `jmp` (Jump-Oriented Programming) or `call` (Call-Oriented Programming).
What if we say that the program can use `jmp` / `call` only to certain instructions?
That's what `Indirect Branch Tracking` does:
a new instruction is added, `endbr`.
Every time a `jmp` or `call` instruction is used, the CPU expects the next instruction that is executed to be `endbr`.
Otherwise, an exception is raised, and the program is terminated.

TODO: it only works for indirect branching - edit is needed

#### Hardware Shadow Stack



### Intel MPX ?
