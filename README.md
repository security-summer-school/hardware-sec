# Hardware Assisted Security

In security, we define the notion of **root of trust**, which is the component of the system that is expected to be secure, and which other components can rely upon.
In the most cases, the root of trust for a system is the hardware.
If the hardware is insecure, everything that relies upon it will also be insecure. How can an operating system guarantee the security of a system, if we can alter what instructions are executed by the CPU?
This brings us to the subject of this (mini-)track: what security measures are implemented in the hardware, how we can use them and how easy is it to break them.

## Outline

- Hardware Encryption
- Hardware Isolation
- Trusted Execution Environments
