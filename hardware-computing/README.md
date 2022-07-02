# Secure Hardware Computing

## Table of Contents

 * [Prerequisites](#prerequisites)
 * [Introduction](#introduction)
 * [AES Instruction Sets](#aes-instruction-sets)
   * [Intel AES-NI](#intel-aes-ni)
     * [Challenge: AES in C](#challenge-aes-in-c)
     * [Challenge: AES in ASM](#challenge-aes-in-asm)
   * [ARM Cryptography Extensions](#arm-cryptography-extensions)
 * [Hardware Random Number Generation](#hardware-random-number-generation)
     * [RDRAND](#rdrand)
     * [RDSEED](#rdseed)
       * [Challenge: RDRAND and RDSEED in Assembly](#challenge-rdrand-and-rdseed-in-assembly)
     * [GCC Compiler Intrinsics](#gcc-compiler-intrinsics)
       * [Challenge: RDRAND and RDSEED in C](#challenge-rdrand-and-rdseed-in-c)
       * [Challenge: Performance Evaluation](#challenge-performance-evaluation)
     * [When to Use RDRAND and RDSEED?](#when-to-use-rdrand-and-rdseed)
  * [Trusted Computing](#trusted-computing)
     * [Trustsed Platorm Module](#trusted-platform-module---tpm)
       * [Exploiting the TPM](#exploiting-the-tpm)
       * [Tutorial: Sign with RSA](#tutorial-sign-with-rsa)
       * [Challenge: AES Decryption](#challenge-aes-decryption)
       * [Tutorial: Seal Arbitrary Data](#tutorial-seal-arbitrary-data)
  * [Resources](#resources)
  * [Further Reading](#further-reading)

## Prerequisites

For this session you should have some knowledge about `AES`, `SIMD instructions` from Intel, and `x86_64` assembly.
For [AES](https://github.com/security-summer-school/hardware-sec/tree/main/helpers/aes.md) and [SIMD](https://github.com/security-summer-school/hardware-sec/tree/main/helpers/simd-instructions.md), you can go through the provided helpers.
For the x86_64 assembly, please review the **Security Essentials** [Assembly Language](https://github.com/security-summer-school/essentials/tree/master/assembly-language) and [Taming the Stack](https://github.com/security-summer-school/essentials/tree/master/taming-the-stack) sessions.

## Introduction

In terms of computing, the tendencies are to require more information to be processed, as fast as possible and as secure as possible.
And usually, the best way to ensure the maximum computation speed is to move the processing from software to hardware.
That's why most systems have specialised hardware components, that are able to do some of the most common operations: checksum computing for network packets, basic routing, encrypting and decrypting, hashing, random number generation.
Some of these operations are critical in ensuring the security of a system.
In this session we will explore some of those:
 * Encryption and decryption on Intel and ARM CPUs, in particular the AES algorithm
 * Random number generation

After studying these concepts, we have one more topic: can we trust the hardware?
Many people have posed this question, including tech giants.
Those questions resulted in the creation of the **Trusted Computing** concept.
We'll see what it is and how it is implemented. 

## AES Instruction Sets

One of the most common algorithms used for encryption and decryption is AES (Advanced Encryption Standard).
As its name suggests, AES is the standard for securing data that cannot (should not) be intercepted over a network.
It is used to encrypt certain files, whole disks, by password managers to store passwords, and other uses.
Being used so much, it was only natural that the most efficient implementation was searched.
And, generally, the most efficient implementation of an algorithm is directly in the hardware.

A hardware implementation has one more benefit: it is more secure.
If done by software, while being secure against brute-force attacks, AES can be broken using side-channel attacks.
The hardware implementation is much harder to be affected by such attacks.

Intel, AMD, ARM, Risc-V, SPARC, IBM have AES instruction sets implemented in hardware.
We will mailny discuss the implentations from Intel, and mention the one from ARM.=

### Intel AES-NI

In 2008, Intel proposed an AES instruction set for the x86_64 CPUs, AES-NI (Advanced Encryption Standard New Instructions).
AMD also implemented the instruction set for their CPUs.

The instruction set provides 2 instructions for encrypting, 2 for decrypting, and 2 for key generation:
 * `AESENC` performs an encryption round
 * `AESENCLAST` performs the last encryption round
 * `AESDEC` performs a decryption round
 * `AESDECLAST` performs the last decryption round
 * `AESKEYGENASSIS` aids in the key expansion phase of the algorithm
 * `AEIMC` prepares a key for being used in decryption

All the instructions use XMM registers, which are part of the SSE / AVX instruction sets.

In the case of the first 4 and the 6th instructions, 2 XMM registers are used: the first one stores the intermediary state of the cyphertext, and the second one the key used in that round.

For those of you that don't like the Assembly language, there are good news.
The AES-NI instructions can be also used in C, not only in Assembly, with the so-called [intrinsic functions](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#othertechs=AES).

The tasks for this part will be straight-forward:

#### 1. Challenge: AES in C

You are provided with an implementation of AES encryption, in C.
Complete the code with the decryption implementation, obtain the flag and submit it.

You can find the task resources [here](https://github.com/security-summer-school/hardware-sec/tree/hardware-computing/hardware-computing/activities/aes-in-c).

#### 2. Challenge: AES in ASM

You are provided with an implementation of AES encryption, in Assembly.
Complete the code with the decryption implementation, obtain the flag and submit it.

You can find the task resources [here](https://github.com/security-summer-school/hardware-sec/tree/hardware-computing/hardware-computing/activities/aes-in-assembly).

### ARM Cryptography Extensions

While Intel and AMD provide the AES-NI instruction set with all their newer processors, ARM provides such instructions as an optional extension module, called the "Cryptography Extension".
This module is available with the ARMv8 base Instruction Set Architecture.
It doesn't support all operations associated with AES encryption and decryption, like aiding key generation.
But, in contrats with Intel, ARM provides instructions that can be used for SHA hash computing.
For AES, the following instructions are provided:
 * `AESE`, which performs a single encryption round
 * `AESD`, which performs a single decryption round
 * `AESMC`, which performs the `MixColumns` operation
 * `AESIMC`, which performs the `InverseMixColumns` operation

ARM also provides compiler intrinsic functions for the assembly instructions.

We won't go into ARM AES this session, but, if you feel up to the challenge, you can try to use the ARM intrinsics.
Documentation can be found in the [Further Reading](#further-reading) section.

## Hardware Random Number Generation

Randomness, also called entropy, is essential in security, being used in generating encryption and decryption keys.
However, ensuring true randomness on a system is a difficult task, which is subject of many research papers.
Usually, random numbers are generated by a **(Pseudo)Random Number Generator - (P)RNG** component on the system, using hardware and software events that can't be easily predicted: cpu frequency at a given time, the number of interrupts, hashes of network packets, movement of the mouse, etc.
Those RNG components are most of the times software.
Considering this, Intel has implemented hardware RNGs on their processors, which are more secure than their software counterparts, being hard to predict.
The values returned by those RNGs can be read using the `RDRAND` and `RDSEED` instructions.

### RDRAND

`RDRAND` is the first implemented instruction on the Intel CPUs.
At the moment of its apparition, it wasn't secure, as it could be intercepted by side-channel attacks.
One such attack is the _CrossTalk_, designated as [CVE-2020-0543](https://www.intel.com/content/www/us/en/security-center/advisory/intel-sa-00320.html).
The reported vulnerabilities were patched, and `RDRAND` is considered secure nowadays. 

Intel guarantees that the generated random number cannot be predicted by an attacker that has finite resources.
That means that the effort needed to predict the number is a lot more higher than the benefits of predicting the random number, but it theoretically could be predicted.
It also isn't a true random number, but a pseudo-random number.

Using `RDRAND` is fairly easy: after calling `rdrand`, a number will be returned in the register provided as argument.
The number is a valid random number if the Carry Flag is not 0 after the instruction executes.

### RDSEED

`RDSEED` is the slower and harder to predict variant of `RDRAND`.
If `RDRAND` can be theoretically predicted, Intel guarantees that the number generated by `RDSEED` cannot be predicted by an attacker with infinite resources.

#### Challenge: RDRAND and RDSEED in Assembly

This task is very easy: generate some random numbers using the `rdrand` and `rdseed` instructions, in assembly.
Use the `$ make run` command to run your program.
It emulates the rdrand and rdseed commands, in case they are not supported on your system.

You can find the task resources [here](https://github.com/security-summer-school/hardware-sec/tree/hardware-computing/hardware-computing/activities/rdrand-and-rdseed-in-assembly).

### GCC Compiler Intrinsics

As most special hardware features, intrinsic functions are implemented in C, as wrappers to `RDRAND` and `RDSEED`.
Those are `_rdrand<size>_step()` and `_rdseed<size>_step()`, where "size" can be 16, 32, or 64.

#### Challenge: RDRAND and RDSEED in C

Again, a simple task. Generate some random numbers in C, using the compiler [intrinsic functions](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#othertechs=RDRAND,RDSEED).

You can find the task resources [here](https://github.com/security-summer-school/hardware-sec/tree/hardware-computing/hardware-computing/activities/rdrand-and-rdseed-in-c).

#### Challenge: Performance Evaluation

Read some values from `/dev/urandom`, and compare the time it takes to do so with the time it takes to read the same number of values using `rdrand`.

You can find the task resources [here](https://github.com/security-summer-school/hardware-sec/tree/hardware-computing/hardware-computing/activities/performance-evaluation).

### Exploiting RDRAND and RDSEED?

Theoretically, there are exploits, that can leak the values of RDRAND and RDSEED.
But the attack is very hard to replicate, so we won't do it.
If you want to have a go at it, you can read the article describing CrossTalk, in the Resources section.
Contact your trainer for details on how to do it.

### When to Use RDRAND and RDSEED?

There are software solutions that are much faster than the hardware RNGs provided by Intel, so why use them?
The RDRAND and RDSEED instructions perform better when cryptographically secure random numbers are needed.

## Trusted Computing

The concept of **Trusted Computing** was introduced by Trusted Computing Group, and it aims to give the hardware control over waht software can be run on it, by being able to refuse to run unsigned software.
This enforcement means that the system will most of the times behave as expected.
For a system to fall under the Trusted Computing standard, it needs to have some components:
  * secure input and output - the sensitive data must not be able to be intercepted by a third party
  * an endorsment key - a public / private key, created at manufacture time, used to prove the identity of the system; the private key must never leave the chip where it is stored
  * memory curtailing - the system must provide full isolation of some sensitive areas of memory, blocking even the Operating System from accessing them
  * sealed storage - the system must provide a storage space, where private data can be stored and extracted only at a certain software-hardware configuration state
  * remote attestation - the system must allow authorised parties to detect changes in the system.
  For example, any changes done to a software by the user and not approved by the vendor, can be detected, using a certificate generated by the hardware
  * trusted third party - to maintain annonimity, a system will not communicate to another system that it's running verified hardware and software directly, as this may allow the receiver of this information to identify the sender.
  Instead, a third party will verify that the first system is non-altered, and will relate that to the second system

All of those concepts are included on most of today's systems, using a hardware chip, called a **Trusted Platform Module**.	  

### Trusted Platform Module - TPM

The TPM is ussually a chip, residing on the motherboard.
It must implement the Trusted Computing standard, meaning that it must implement the 6 components from above.
The current version of the TPM is 2.0, and is mandatory in order to run some software, like Windows 10.
The user can interact with the TPM using special libraries.
For today's session, we will use bash commands, and the [tpm2-tools](https://github.com/tpm2-software/tpm2-tools) suite.
Particularly usefull is the [man folder](https://github.com/tpm2-software/tpm2-tools/tree/master/man).
You can use a [Docker image](https://github.com/security-summer-school/hardware-sec/tree/main/hardware-computing/resources/tpm-docker), with the tools already installed.

#### Exploiting the TPM

If exploiting the RDRAND and RDSEED instructions seems hard and unreliable, TPM is almost impossible to be exploited without direct access to the chip.
You can find the CVEs in the [Further Reading](#further-reading) section.
So, for today's session, we won't try to break the TPM, but we will instead use it to perform several operations.

#### Tutorial: Sign with RSA

In order to use a key to sign a certificate (or anything else) with RSA, the keys must be created first.
In TPM, a RSA key-pair is an object, and a child of a primary object.
The first step is to create a primary object, using the following command: (must be root)

```
tpm2_createprimary -C e -c primary.ctx
```

The `-C e` argument creates the primary object under the `TPM_RH_ENDORSEMENT` hierarchy, which is controlled by the administrator (the user).
The `-c` option is used to provide a place to store the context of the object, which is used to identify that object in the future.

Then, we create a child object, the RSA key-pair.

```
tpm2_create -G rsa -u rsa.pub -r rsa.priv -C primary.ctx
```

Before we can use the key-pair, we must load it into the TPM.

```
tpm2_load -C primary.ctx -u rsa.pub -r rsa.priv -c rsa.ctx
```

Now we can use the loaded key-pair to sign (encrypt) and verify the signature of any file.

```
tpm2_sign -c rsa.ctx -g sha256 -o sig.rssa sign_me.dat
```

The last command generates a signed file, `sig.rssa`.
To verify that the signature is valid, we can use

```
tpm2_verifysignature -c rsa.ctx -g sha256 -s sig.rssa -m sign_me.dat
```

If the signature is valid, no output will be returned.
Try to modify `sign_me.dat`, and recheck the signature.

You can find the task resources [here](https://github.com/security-summer-school/hardware-sec/tree/hardware-computing/hardware-computing/activities/tpm-rsa).

#### Challenge: AES Decryption

Someone left an encrypted file here, along with the AES key that was used.
Decrypt the file, find the flag and submit it.

Use `man tpm2` to find the commands needed.

You can find the task resources [here](https://github.com/security-summer-school/hardware-sec/tree/hardware-computing/hardware-computing/activities/tpm-aes).

#### Tutorial: Seal Arbitrary Data

TPM allows the user to seal data inside the hardware module.
The maximum size of the data that can be sealed is 128 bytes.
To seal data, we use the following command.

```
tpm2_create -C primary.ctx -i seal_me.dat -u seal.pub -r seal.priv -c seal.ctx
```

We can add an authorization policy to the sealed object, based on the `Platform Configuration Registers` - PCR.
Those registers are set up at boot time, and can be used to verify that the system is a valid state, before being able to unseal an object.

To read the PCR, we can use 

```
tpm2_pcrread -o pcr.bin sha256:0,1,2,3
```

This command reads from the registers from 0 to 4, from the `sha256` bank of registers, and stores the results in `pcr.bin`.
Then we can create a policy, that allows unsealing only if `sha256:0,1,2,3` are in the state described in `pcr.bin`.

```
tpm2_createpolicy -Q --policy-pcr -l sha256:0,1,2,3 -f pcr.bin -L pcr.policy
```

Then, seal the data using the new policy

```
tpm2_create -C primary.ctx -i seal_me.dat -u seal.pub -r seal.priv -L pcr.policy -c seal.ctx
```

To unseal the data, we can use

```
tpm2_unseal -c seal.ctx -p pcr:sha256:0,1,2,3
```

You can find the task resources [here](https://github.com/security-summer-school/hardware-sec/tree/hardware-computing/hardware-computing/activities/tpm-seal).

## Resources

[Intel AES-NI White Paper](https://www.intel.com/content/dam/doc/white-paper/advanced-encryption-standard-new-instructions-set-paper.pdf)

[Intel AES-NI Intrinsic Functions](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#othertechs=AES)

[TPM 2.0 Specification](https://trustedcomputinggroup.org/resource/tpm-library-specification/)

## Further Reading

1. [Windows ARM AES Intrinsics](https://docs.microsoft.com/en-us/dotnet/api/system.runtime.intrinsics.arm.aes?view=net-6.0)

2. [Linux ARM AES Intrinsics](https://developer.arm.com/architectures/instruction-sets/intrinsics/#f:@navigationhierarchiesinstructiongroup=[Cryptography,AES])

3. [Intel DRNG Implementation](https://www.intel.com/content/dam/develop/external/us/en/documents/441-intel-r-drng-software-implementation-guide-final-aug7.pdf)

4. [CrossTalk Paper](https://download.vusec.net/papers/crosstalk_sp21.pdf)

5. [TPM Local Security Vulnerability - CVE-2018-6622](https://www.cvedetails.com/cve/CVE-2018-6622/)

6. [TPM-FAIL - TPM Timing Attack - CVE-2019-11090 and CVE-2019-16863](https://tpm.fail/tpmfail.pdf)

7. [TPM Device Driver Information Disclosure Vunerability - CVE 2021-1656](https://msrc.microsoft.com/update-guide/vulnerability/CVE-2021-1656)

8. [Infineon TPM Vulnerability - CVE-2017-15361](https://www.dell.com/support/kbdoc/ro-ro/000144419/infineon-trusted-platform-module-tpm-vulnerability-cve-2017-15361-impact-status-on-dell-products)
