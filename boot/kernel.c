void kernel_main(void) {
    char *video = (char*) 0xB8000;
    const char *msg = "AuraOS";
    int i = 0;
    while (msg[i]) {
        video[i * 2] = msg[i];
        video[i * 2 + 1] = 0x1F;
        i++;
    }
    while (1);
}