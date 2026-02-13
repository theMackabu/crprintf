# crprintf

printf with inline color tags and variables, powered by a register-based VM

## Usage

```c
#include <crprintf.h>

// basic colored output
crprintf("<red>error:</red> something went wrong\n");
crprintf("<bold><cyan>info:</cyan></bold> hello %s\n", name);

// hex colors
crprintf("<#ff8800>orange text</#ff8800>\n");

// padding
crprintf("  <pad=18><green>%s</green></pad> %s\n", cmd->name, cmd->desc);
```

## Building

```bash
meson setup build
meson compile -C build
meson install -C build
```

## Meson Subproject

Add to your `subprojects/crprintf.wrap`:

```ini
[wrap-git]
url = https://github.com/themackabu/crprintf.git
revision = head
depth = 1
```

Then in your `meson.build`:

```meson
crprintf_proj = subproject('crprintf')
crprintf_dep = crprintf_proj.get_variable('crprintf_dep')

executable('myapp',
  'main.c',
  dependencies: crprintf_dep
)
```

## Configuration

Configure library options when building:

```bash
meson setup build -Dc_std=c11 -Dwarning_level=1
```

Or as a subproject:

```meson
crprintf_proj = subproject('crprintf',
  default_options: ['c_std=c11', 'warning_level=1']
)
```

## API

### Configuration

- `crprintf_set_color(bool)` - Enable/disable color output
- `crprintf_get_color()` - Get color state
- `crprintf_set_debug(bool)` - Enable debug disassembly
- `crprintf_set_debug_hex(bool)` - Enable hex dump debug
- `crprintf_var(name, value)` - Set a variable for use in format strings

### Printing

- `crprintf(fmt, ...)` - Print to stdout with colors
- `crfprintf(stream, fmt, ...)` - Print to file with colors
- `crsprintf(buf, size, fmt, ...)` - Print to buffer

### Supported Tags

- `<red>` `<green>` `<yellow>` `<blue>` `<magenta>` `<cyan>` `<white>` `<black>`
- `<gray/grey>` `<bright_red>` `<bright_green>` ... etc
- `<bg_red>` `<bg_green>` ... `<bg_#RGB>` `<bg_#RRGGBB>`
- `<bold>` `<dim>` `<ul>` (underline)
- `<bold_red>` `<dim_cyan>` etc - combine styles with underscores
- `<bold+red>` `<dim+cyan+bg_blue>` etc - combine styles with +
- `<#RRGGBB>` or `<#RGB>` for arbitrary 24-bit foreground colors
- `<pad=N>` ... `</pad>` - right-pad contents to N visible columns
- `<br/>` - emit a newline, `<br=N/>` - emit N newlines
- `<rpad=N>` ... `</rpad>` - left-pad (right-align) contents to N visible columns
- `<space=N/>` - emit N spaces
- `<gap=N/>` - emit N spaces (alias for space)
- `<let name=style1+style2+...>` or `<let name=style1, name2=style2...>` - define a named style variable
- `{let name=style1+style2+...}` or `{let name=style1, name2=style2...}` - alternative syntax
- `{let label='hello'}` or `<let label="world"/>` - quotes are stripped from value
- `<$name>` to apply a variable as a style, `{name}` to emit its value as literal text
- `{~name}` for lowercase, `{^name}` for uppercase
- `{~'string'}` for lowercase literal, `{^'string'}` for uppercase literal
- `</tagname>` or `</>` to reset (pops one level)
- `<reset/>` to reset all styles (clears entire stack)
- `<<` and `>>` to emit literal `<` and `>`
- `%%` to emit a literal `%`
