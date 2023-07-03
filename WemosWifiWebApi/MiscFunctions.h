int availableMemory() {
  int size = 81920;  // Use 2048 with ATmega328
  byte *buf;

  while ((buf = (byte *)malloc(--size)) == NULL)
    ;

  free(buf);

  return size;
}