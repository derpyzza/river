standard library functions sketch

# Mem
```

// used like mem.alloc(int), mem.alloc(char*, 32), mem.alloc(GameObject*, 128, allocator: arena);
fn alloc(type: T, items = 1, allocator = default): [*]any;
fn free ([*]any);
```

# Io

# Math


# Regex

# Image

# Time

# Os

# String
Unicode ( utf-8 ) string library

# Opts
command line argument parsing library

# Json

# Dll loader

# Window
abstract platform indepentant windowing library.
basically just gives you a window, yaaaay.

# Graphics
river is made with gamedev in mind, so it makes sense to have a small graphics library built-in.
might just be a very primitive setup that draws simple shapes and such, with an opengl1 or LÖVE2D style api.
prolly might just be software rendered so that i don't have to embed an entire proper hardware-rendered rendering engine into the stdlib. that's a job for third-party libraries.

```

fun begin_drawing();
fun end_drawing();
fun clear_screen(colour: Colour);
fun set_draw_mode(mode: DrawMode);
fun set_colour(colour: Colour);
fun set_stroke_colour(colour: Colour);
fun set_stroke_width(w: int);
fun draw_rect(x, y, w, h: int, mode: DrawMode);
fun draw_circle(x, y, r: int, mode: DrawMode);
fun draw_polygon(points: []Points, mode: DrawMode);
fun draw_image(img: Image, x, y: int);
  
```


