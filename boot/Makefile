# ===== AuraOS UEFI Build =====
EFIINC = /usr/include/efi
EFILIB = /usr/lib

CFLAGS = -I$(EFIINC) -I$(EFIINC)/x86_64 -I$(EFIINC)/protocol \
         -fno-stack-protector -fpic -fshort-wchar -mno-red-zone \
         -Wall -O2

OBJS = main.o

all: bootx64.efi image

# ELF
bootx64.so: $(OBJS)
	ld -shared -Bsymbolic -L$(EFILIB) -T$(EFILIB)/elf_x86_64_efi.lds \
	   $(OBJS) -lgnuefi -lefi -o $@

# ELF → PE32+ (попробуем разные способы)
bootx64.efi: bootx64.so
	objcopy \
	  -j .text -j .sdata -j .data -j .dynamic -j .dynsym \
	  -j .rel -j .rela -j .reloc \
	  --output-target=pei-x86-64 \
	  --subsystem=10 \
	  $< $@

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

# Копируем в ESP
esp/EFI/BOOT/BOOTX64.EFI: bootx64.efi
	mkdir -p esp/EFI/BOOT
	cp bootx64.efi esp/EFI/BOOT/BOOTX64.EFI

# FAT32 образ
image: esp/EFI/BOOT/BOOTX64.EFI
	dd if=/dev/zero of=auraos-uefi.img bs=1M count=64
	mkfs.vfat -F 32 auraos-uefi.img
	mmd -i auraos-uefi.img ::/EFI
	mmd -i auraos-uefi.img ::/EFI/BOOT
	mmd -i auraos-uefi.img ::/EFI/AuraOS
	mcopy -i auraos-uefi.img bootx64.efi ::/EFI/BOOT/BOOTX64.EFI

run: image
	cp /usr/share/edk2/x64/OVMF_VARS.4m.fd OVMF_VARS.fd
	qemu-system-x86_64 \
	  -drive if=pflash,format=raw,readonly=on,file=/usr/share/edk2/x64/OVMF_CODE.4m.fd \
	  -drive if=pflash,format=raw,file=OVMF_VARS.fd \
	  -drive format=raw,file=auraos-uefi.img,if=ide \
	  -m 512

clean:
	rm -f *.o *.so *.efi *.img OVMF_VARS.fd
	rm -f esp/EFI/BOOT/BOOTX64.EFI