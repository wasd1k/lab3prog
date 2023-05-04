#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t generate_number() {
  const int r = rand();
  const int p = r % 100;
  if (p < 90) {
    return r % 128;
  }
  if (p < 95) {
    return r % 16384;
  }
  if (p < 99) {
    return r % 2097152;
  }
  return r % 268435455;
}

size_t encode_varint(uint32_t value, uint8_t *buf) {
  assert(buf != NULL);
  uint8_t *cur = buf;
  while (value >= 0x80) {
    const uint8_t byte = (value & 0x7f) | 0x80;
    *cur = byte;
    value >>= 7;
    ++cur;
  }
  *cur = value;
  ++cur;
  return cur - buf;
}

uint32_t decode_varint(const uint8_t **bufp) {
  const uint8_t *cur = *bufp;
  uint8_t byte = *cur++;
  uint32_t value = byte & 0x7f;
  size_t shift = 7;
  while (byte >= 0x80) {
    byte = *cur++;
    value += (byte & 0x7f) << shift;
    shift += 7;
  }
  *bufp = cur;
  return value;
}

int main() {
  size_t size = 0;
  FILE *fp = fopen("compressed.dat", "wb");
  FILE *unfp = fopen("uncompressed.dat", "wb");

  for (int i = 0; i < 1000000; i++) {
    uint32_t number = generate_number();
    fwrite(&number, sizeof(uint32_t), 1, unfp);
    uint8_t buf[4];
    size = encode_varint(number, buf);
    fwrite(buf, 1, size, fp);
    const uint8_t *cur_uncomp = buf;
    uint32_t value = decode_varint(&cur_uncomp);
    if (value != number) {
      printf("ERROR");
    }
  }
  fclose(fp);
  fclose(unfp);
  
  FILE *file = fopen("compressed.dat", "rb");
  fseek(file, 0, SEEK_END);
  uint32_t sizef = ftell(file);
  printf("Размер сжатого: %d\n", sizef);
  fclose(file); 
  FILE *fileU = fopen("uncompressed.dat", "rb");
  fseek(fileU, 0, SEEK_END);
  sizef = ftell(fileU);
  printf("Размер несжатого: %d\n", sizef);
  fclose(fileU);
  
  return 0;
}
