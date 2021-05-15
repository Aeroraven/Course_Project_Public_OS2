FLAGS = -finline -faggressive-loop-optimizations -fvar-tracking -fschedule-fusion -fauto-inc-dec -fdce -fdse -fivopts -fjump-tables -frename-registers -fsched-spec
FLAGS2 = -fweb -fschedule-fusion
load:	
	nasm -o boot.bin -I boot/ boot/boot.asm 
	nasm -o loader.bin -I boot/ boot/loader.asm
	nasm -f elf -o kernel.o -I kernel/ kernel/kernel.asm
	nasm -f elf -o kernel_asmfunc.o -I kernel/ kernel/Kernel_AsmFunc.asm
	gcc -m32 -c -o kernel_c.o kernel/Kernel_C.c -fno-stack-protector -fno-builtin  -I kernel/ -masm=intel $(FLAGS) $(FLAGS2)
	gcc -m32 -c -o kernel_globalvar.o kernel/kernel_GlobalVar.c -fno-stack-protector -O1 -fno-builtin -I kernel/ 

	ld -m elf_i386 -s -Ttext 0x1000400  -o kernel.bin kernel.o kernel_asmfunc.o kernel_c.o kernel_globalvar.o

	dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img vdisk/
	sudo rm -f vdisk/loader.bin
	sudo cp loader.bin vdisk/
	sudo cp kernel.bin vdisk/
	sudo umount vdisk/

clean:
	rm -f kernel.o
	rm -f kernel_asmfunc.o
	rm -f kernel_c.o
	rm -f kernel.bin
	sudo mount -o loop a.img vdisk/
	sudo rm -f vdisk/loader.bin
	sudo rm -f vdisk/kernel.bin
	sudo umount vdisk/

writeboot:
	nasm -o boot.bin boot.asm
	dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc

umount:
	sudo umount vdisk/