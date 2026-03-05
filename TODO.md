
- Detect binary type in env_defines.h:
  BIN_RUNNABLE,
  BIN_DYNAMIC_LIB,
  BIN_STATIC_LIB,
  BIN_OBJ_LIB,
  Throw error if none is defined...
  When something links with the library in CMAKE, 
  is it possible to define these automatically?

- Fixing entry:
  The header entry.h is correct.
  Fix entry.c with proper function calls
  Ensure module.h/module.c call the proper mod functions 
  When the user defines ENTRY_TYPE_MOD, can you make sure the current build
  is actually BIN_DYNAMIC_LIB?

- Hash map (hash_map.h, hash_map.c):
  I see hash map defines it's own hashing functions, libmath2 already defines
  some. We dont want to repeat ourselves and reimplement.

- Keyboard:
  At the moment we cant check if a key is pressed, relased, we cant fetch 
  its repeat count...
  Also define a proper keymod enum, and create functions to check for keys with/without keymods

- Mouse & gamepad buttons:
  We cant check if buttons are pressed, released, ....