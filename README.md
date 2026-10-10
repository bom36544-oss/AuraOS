# AuraOS

**Своя операционная система, написанная с нуля.**
Не Linux, не Windows, не macOS — полностью своя.

---

## О проекте

AuraOS — это уникальная операционная система, разрабатываемая с нуля.
Никаких готовых ядер, никаких дистрибутивов Linux — только свой код.

**Цель:** создать полноценную ОС с собственным ядром, файловой системой,
графическим интерфейсом и уникальными фишками.

---

## Что уже готово

### Загрузчик (Aura Boot)
- BIOS Boot — 16-бит (ASM)
- Protected mode — 16 → 32 (stage2)
- Long mode — 32 → 64 (stage3)
- Paging — 4-уровневые таблицы
- GDT 64-бит — свои дескрипторы

### Ядро
- Aura Kernel 64-бит — ядро на C
- VGA — вывод на экран
- Анимация логотипа — снизу вверх
- Меню загрузки — AuraOS / Safe Mode
- Рамка меню — красивая графика
- Клавиатура — стрелки + Enter
- Адаптивный интерфейс — под любой экран

---

## Что в планах

- Звук — HDA (джек + динамики)
- AuraFAT — своя файловая система
- Aura GUI — графический интерфейс
- Мышь — USB (курсор «A» с пульсацией)
- Aura Store — магазин приложений
- .aurapp — свой формат приложений
- Кастомизация — через JSON

---

## Архитектура

| Компонент | Технология |
|---|---|
| Загрузчик (boot) | ASM (16-бит, BIOS) |
| Stage 2 | ASM (16 → 32, protected mode) |
| Stage 3 | ASM (32 → 64, long mode) |
| Ядро | C (64-бит) |
| Вывод | VGA (текстовый режим) |
| Клавиатура | Порты 0x60, 0x64 |
| Сборка | NASM + GCC + LD |

---

## Структура проекта

    AuraOS/
    └── boot/
        ├── boot.asm          # 16-бит (BIOS)
        ├── stage2.asm        # 16 → 32 (protected mode)
        ├── stage3.asm        # 32 → 64 (long mode)
        ├── kernel64.c        # 64-бит ядро
        ├── linker64.ld       # 64-бит линкер
        └── Makefile          # сборка

---

## Сборка

### Требования
- NASM — ассемблер
- GCC — компилятор (с поддержкой -m64)
- LD — линкер (с -m elf_x86_64)
- QEMU — эмулятор (qemu-system-x86_64)

### Установка (Arch-based: CharyOS, CachyOS)

    sudo pacman -S nasm gcc make qemu-full git base-devel

### Команды

    cd boot
    make
    make run

### Запуск вручную

    qemu-system-x86_64 -fda auraos.img -display gtk -m 512

---

## Скриншот

      A   U   U RRRR   A   BBBB   OO   OO  TTTTT
     A A  U   U R   R A A  B   B O  O O  O   T
    AAAAA U   U RRRR  AAA  BBBB  O  O O  O   T
    A   A U   U R R   A A  B   B O  O O  O   T
    A   A  UUU  R  R  A   A BBBB  OO   OO    T

            +----------------------------+
            |         > AuraOS <         |
            |       > Safe Mode <        |
            +----------------------------+

---

## Лицензия

См. файл LICENSE.

**AuraOS License** — своя, уникальная.

---

## Контакты

- GitHub: [bom36544-oss/AuraOS](https://github.com/bom36544-oss/AuraOS)
- Автор: bom36544-oss

---

**AuraOS — своя, уникальная, мощная.**