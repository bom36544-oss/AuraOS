all: auraos.img

boot.bin: boot.asm
	nasm -f bin boot.asm -o boot.bin

stage2.bin: stage2.asm
	nasm -f bin stage2.asm -o stage2.bin
	truncate -s %512 stage2.bin

kernel.o: kernel.c
	gcc -m32 -ffreestanding -fno-pie -fno-stack-protector -c kernel.c -o kernel.o

kernel.elf: kernel.o
	ld -m elf_i386 -T linker.ld -o kernel.elf kernel.o

kernel.bin: kernel.elf
	objcopy -O binary kernel.elf kernel.bin
	truncate -s %512 kernel.bin

auraos.img: boot.bin stage2.bin kernel.bin
	cat boot.bin stage2.bin kernel.bin > auraos.img
	truncate -s %512 auraos.img

run: auraos.img
	qemu-system-i386 -drive format=raw,file=auraos.img -display gtk -m 512

clean:
	rm -f *.bin *.img *.o *.elf