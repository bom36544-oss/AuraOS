all: auraos.img

boot.bin: boot.asm
	nasm -f bin boot.asm -o boot.bin

stage2.bin: stage2.asm
	nasm -f bin stage2.asm -o stage2.bin
	truncate -s %512 stage2.bin

stage3.bin: stage3.asm
	nasm -f bin stage3.asm -o stage3.bin
	truncate -s %512 stage3.bin

kernel64.o: kernel64.c
	gcc -m64 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -c kernel64.c -o kernel64.o

kernel64.elf: kernel64.o
	ld -m elf_x86_64 -T linker64.ld -o kernel64.elf kernel64.o

kernel64.bin: kernel64.elf
	objcopy -O binary kernel64.elf kernel64.bin
	truncate -s %512 kernel64.bin

auraos.img: boot.bin stage2.bin stage3.bin kernel64.bin
	cat boot.bin stage2.bin stage3.bin kernel64.bin > auraos.img
	truncate -s %512 auraos.img

run: auraos.img
	qemu-system-x86_64 -drive format=raw,file=auraos.img -display gtk -m 512

clean:
	rm -f *.bin *.img *.o *.elf