load:	
	nasm -o boot.bin boot.asm
	nasm -o loader.bin loader.asm
	nasm -f elf -o kernel.o kernel.asm
	nasm -f elf -o kernel_asmfunc.o Kernel_AsmFunc.asm
	gcc -m32 -c -o kernel_c.o Kernel_C.c

	ld -m elf_i386 -s -Ttext 0x30400 -o kernel.bin kernel.o kernel_asmfunc.o kernel_c.o

	dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img vdisk/
	sudo rm -f vdisk/loader.bin
	sudo cp loader.bin vdisk/
	sudo cp kernel.bin vdisk/
	sudo umount vdisk/

clean:	
	sudo mount -o loop a.img vdisk/
	sudo rm -f vdisk/loader.bin
	sudo rm -f vdisk/kernel.bin
	sudo umount vdisk/

load2:
	sudo mount -o loop a.img vdisk/
	sudo cp loader.bin vdisk/
	sudo cp kernel.bin vdisk/
	sudo umount vdisk/

writeboot:
	nasm -o boot.bin boot.asm
	dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc

umount:
	sudo umount vdisk/