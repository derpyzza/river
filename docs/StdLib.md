standard library functions sketch

# Mem
```

// used like mem.alloc(int), mem.alloc(char*, 32), mem.alloc(GameObject*, 128, allocator: arena);
fn alloc(type: T, items = 1, allocator = default): [*]any;
fn free ([*]any);
```

# Io

# Math

# String
Unicode ( utf-8 ) string library

# Json

