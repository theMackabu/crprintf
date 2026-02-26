#include <crprintf.h>
#include <stdio.h>
#include <string.h>

int main(void) {
  crprintf_var("kw", "bold+blue");
  crprintf_var("str", "green");
  crprintf_var("num", "yellow");

  printf("--- stateful multi-line rendering ---\n\n");

  // simulate a multi-line REPL input where a string spans lines
  crprintf_state *state = crprintf_state_new();
  char buf[512];

  // line 1: opens a template literal, never closes it
  crsprintf_stateful(buf, sizeof(buf), state,
    "<$kw>const</> x = <$str>`hello");
  printf("line 1: %s\x1b[0m\n", buf);

  // line 2: continues inside the string context (state carries forward)
  crsprintf_stateful(buf, sizeof(buf), state,
    "  world ${<$num>2</> + <$num>2</>}");
  printf("line 2: %s\x1b[0m\n", buf);

  // line 3: closes the template literal
  crsprintf_stateful(buf, sizeof(buf), state,
    "`</>");
  printf("line 3: %s\x1b[0m\n", buf);

  crprintf_state_free(state);

  printf("\n--- incremental recompile ---\n\n");

  // simulate keystroke-by-keystroke editing with incremental recompile
  crprintf_compiled *prog = NULL;

  const char *versions[] = {
    "<red>hello</red> world",
    "<red>hello</red> world!",
    "<red>hello</red> world!!",
    "<red>hello</red> brave world!!",
  };

  for (int i = 0; i < 4; i++) {
    prog = crprintf_recompile(prog, versions[i]);
    crsprintf_compiled(buf, sizeof(buf), NULL, prog);
    printf("v%d: %s\n", i + 1, buf);
  }

  crprintf_compiled_free(prog);

  printf("\n--- state clone + compare ---\n\n");

  crprintf_state *a = crprintf_state_new();
  crprintf_state *b = crprintf_state_clone(a);

  printf("empty states equal: %s\n", crprintf_state_eq(a, b) ? "yes" : "no");

  // mutate a by rendering through it
  crsprintf_stateful(buf, sizeof(buf), a, "<bold><red>open");
  printf("after unclosed tags: %s\n", crprintf_state_eq(a, b) ? "equal" : "different");

  crprintf_state_free(a);
  crprintf_state_free(b);

  return 0;
}
