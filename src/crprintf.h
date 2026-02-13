/*
 * crprintf - printf with inline color tags and variables, powered by a register-based VM
 *
 * Copyright (c) 2026 theMackabu (me@themackabu.dev)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * usage:
 *   crprintf("<red>error:</red> something went wrong\n");
 *   crprintf("<bold><cyan>info:</cyan></bold> hello %s\n", name);
 *   crprintf("<#ff8800>orange text</#ff8800>\n");
 *   crprintf("  <pad=18><green>%s</green></pad> %s\n", cmd->name, cmd->desc);
 *
 * supported tags:
 *   <red> <green> <yellow> <blue> <magenta> <cyan> <white> <black>
 *   <gray/grey> <bright_red> <bright_green> ... etc
 *   <bg_red> <bg_green> ... <bg_#RGB> <bg_#RRGGBB>
 *   <bold> <dim> <ul> (underline)
 *   <bold_red> <dim_cyan> etc - combine styles with underscores
 *   <bold+red> <dim+cyan+bg_blue> etc - combine styles with +
 *   <#RRGGBB> or <#RGB> for arbitrary 24-bit foreground colors
 *   <pad=N> ... </pad>  - right-pad contents to N visible columns
 *   <br/> - emit a newline, <br=N/> - emit N newlines
 *   <rpad=N> ... </rpad> - left-pad (right-align) contents to N visible columns
 *   <space=N/> - emit N spaces
 *   <gap=N/> - emit N spaces (alias for space)
 *   <let name=style1+style2+...> or <let name=style1, name2=style2...> - define a named style variable
 *   {let name=style1+style2+...} or {let name=style1, name2=style2...} - alternative syntax
 *   quoted values: {let label='hello'} or <let label="world"/> - quotes are stripped from value
 *   <$name> to apply a variable as a style, {name} to emit its value as literal text
 *   {~name} for lowercase, {^name} for uppercase
 *   {~'string'} for lowercase literal, {^'string'} for uppercase literal
 *   </tagname> or </> to reset (pops one level)
 *   <reset/> to reset all styles (clears entire stack)
 *   << and >> to emit literal < and >
 *   %% to emit a literal %
 */

#ifndef CRPRINTF_H
#define CRPRINTF_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

void crprintf_set_color(bool enable);
bool crprintf_get_color(void);

void crprintf_set_debug(bool enable);
bool crprintf_get_debug(void);

void crprintf_set_debug_hex(bool enable);
bool crprintf_get_debug_hex(void);

struct program_t *crprintf_compile(const char *fmt);
int crprintf_exec(struct program_t *prog, FILE *stream, ...);
int crsprintf_inner(struct program_t *prog, char *buf, size_t size, ...);

void crprintf_var(const char *name, const char *value);
void crprintf_hexdump(struct program_t *prog, FILE *out);
void crprintf_disasm(struct program_t *prog, FILE *out);

#define _CRPRINTF_INIT(prog, fmt) \
  if (!prog) { \
    prog = crprintf_compile(fmt); \
    if (crprintf_get_debug()) crprintf_disasm(prog, stderr); \
    if (crprintf_get_debug_hex()) crprintf_hexdump(prog, stderr); \
  }
  
#define crprintf(fmt, ...) ({ \
  static struct program_t *_cp_prog_ = NULL; \
  _CRPRINTF_INIT(_cp_prog_, fmt); \
  crprintf_exec(_cp_prog_, stdout, ##__VA_ARGS__); \
})

#define crfprintf(stream, fmt, ...) ({ \
  static struct program_t *_cp_prog_ = NULL; \
  _CRPRINTF_INIT(_cp_prog_, fmt); \
  crprintf_exec(_cp_prog_, stream, ##__VA_ARGS__); \
})

#define crsprintf(buf, size, fmt, ...) ({ \
  static struct program_t *_cp_prog_ = NULL; \
  _CRPRINTF_INIT(_cp_prog_, fmt); \
  crsprintf_inner(_cp_prog_, buf, size, ##__VA_ARGS__); \
})

#endif
