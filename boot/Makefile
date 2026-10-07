all: auraos.img

boot.bin: boot.asm
	nasm -f bin boot.asm -o boot.bin

stage2.bin: stage2.asm
	nasm -f bin stage2.asm -o stage2.bin

auraos.img: boot.bin stage2.bin
	cat boot.bin stage2.bin > auraos.img
	truncate -s %512 auraos.img

run: auraos.img
	qemu-system-i386 -drive format=raw,file=auraos.img -display gtk -m 512

clean:
	rm -f *.bin *.img