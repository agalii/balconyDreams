#ifndef _MACROS_H
#define _MACROS_H

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#define xsprintf(buf, fmt, ...) \
  snprintf(buf, sizeof(buf), fmt, __VA_ARGS__)

#define DEBUG_LOC() {           \
  Serial.print(__func__);       \
  Serial.print("(): ");         \
  Serial.print(__LINE__, DEC);  \
  Serial.print("\n\r");         \
}

#endif /* end of include guard: _MACROS_H */
