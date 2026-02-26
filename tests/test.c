#include <crprintf.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static int test_count = 0;
static int pass_count = 0;

#define TEST(name) void test_##name(void)
#define RUN_TEST(name) do { \
  printf("Running %s... ", #name); \
  test_##name(); \
  test_count++; \
  pass_count++; \
  printf("PASS\n"); \
} while(0)

#define ASSERT_EQ(a, b) do { \
  if ((a) != (b)) { \
    printf("FAIL: %s:%d: expected %d, got %d\n", __FILE__, __LINE__, (int)(b), (int)(a)); \
    return; \
  } \
} while(0)

#define ASSERT_STR_EQ(a, b) do { \
  if (strcmp((a), (b)) != 0) { \
    printf("FAIL: %s:%d: expected \"%s\", got \"%s\"\n", __FILE__, __LINE__, (b), (a)); \
    return; \
  } \
} while(0)

TEST(basic_string) {
  char buf[256];
  int n = crsprintf(buf, sizeof(buf), "hello world");
  ASSERT_EQ(n, 11);
  ASSERT_STR_EQ(buf, "hello world");
}

TEST(format_specifiers) {
  char buf[256];
  crsprintf(buf, sizeof(buf), "num: %d", 42);
  ASSERT_STR_EQ(buf, "num: 42");
  
  crsprintf(buf, sizeof(buf), "str: %s", "test");
  ASSERT_STR_EQ(buf, "str: test");
  
  crsprintf(buf, sizeof(buf), "hex: 0x%x", 255);
  ASSERT_STR_EQ(buf, "hex: 0xff");
  
  crsprintf(buf, sizeof(buf), "float: %.2f", 3.14);
  ASSERT_STR_EQ(buf, "float: 3.14");
}

TEST(color_tags_stripped_in_no_color_mode) {
  char buf[256];
  crprintf_set_color(false);
  crsprintf(buf, sizeof(buf), "<red>hello</red>");
  ASSERT_STR_EQ(buf, "hello");
  crprintf_set_color(true);
}

TEST(padding_right) {
  char buf[256];
  crsprintf(buf, sizeof(buf), "<pad=10>hi</pad>");
  ASSERT_EQ(strlen(buf), 10);
  ASSERT_EQ(buf[0], 'h');
  ASSERT_EQ(buf[1], 'i');
  ASSERT_EQ(buf[2], ' ');
  ASSERT_EQ(buf[9], ' ');
}

TEST(padding_left) {
  char buf[256];
  crsprintf(buf, sizeof(buf), "<rpad=10>hi</rpad>");
  ASSERT_EQ(strlen(buf), 10);
  ASSERT_EQ(buf[0], ' ');
  ASSERT_EQ(buf[7], ' ');
  ASSERT_EQ(buf[8], 'h');
  ASSERT_EQ(buf[9], 'i');
}

TEST(newlines) {
  char buf[256];
  crsprintf(buf, sizeof(buf), "a<br/>b");
  ASSERT_STR_EQ(buf, "a\nb");
  
  crsprintf(buf, sizeof(buf), "a<br=2/>b");
  ASSERT_EQ(strlen(buf), 4);
  ASSERT_EQ(buf[1], '\n');
  ASSERT_EQ(buf[2], '\n');
}

TEST(spaces) {
  char buf[256];
  crsprintf(buf, sizeof(buf), "a<space=3/>b");
  ASSERT_EQ(strlen(buf), 5);
  ASSERT_EQ(buf[1], ' ');
  ASSERT_EQ(buf[2], ' ');
  ASSERT_EQ(buf[3], ' ');
}

TEST(escapes) {
  char buf[256];
  crsprintf(buf, sizeof(buf), "<< >> %%");
  ASSERT_STR_EQ(buf, "< > %");
}

TEST(extra_styles) {
  char buf[256];
  crprintf_set_color(false);
  crsprintf(buf, sizeof(buf), "<i>italic</i> <italic>italic</italic> <strike>strike</strike> <invert>invert</invert>");
  ASSERT_STR_EQ(buf, "italic italic strike invert");
  crprintf_set_color(true);
}

TEST(reset) {
  char buf[256];
  crprintf_set_color(false);
  crsprintf(buf, sizeof(buf), "<red>hello <reset/>world");
  ASSERT_STR_EQ(buf, "hello world");
  crprintf_set_color(true);
}

TEST(variables) {
  char buf[256];
  crprintf_var("myvar", "testvalue");
  crprintf_set_color(false);
  crsprintf(buf, sizeof(buf), "{myvar}");
  ASSERT_STR_EQ(buf, "testvalue");
  crprintf_set_color(true);
}

TEST(buffer_overflow) {
  char buf[8];
  int n = crsprintf(buf, sizeof(buf), "hello world this is long");
  ASSERT_EQ(n, 24);
  ASSERT_EQ(buf[7], '\0');
  ASSERT_EQ(buf[0], 'h');
  ASSERT_EQ(buf[5], ' ');
}

TEST(state_new_is_clean) {
  crprintf_state *s = crprintf_state_new();
  crprintf_state *empty = crprintf_state_new();
  ASSERT_EQ(crprintf_state_eq(s, empty), true);
  crprintf_state_free(s);
  crprintf_state_free(empty);
}

TEST(stateful_matches_non_stateful) {
  char buf_normal[256], buf_stateful[256];
  crprintf_set_color(false);
  crprintf_state *state = crprintf_state_new();

  crsprintf(buf_normal, sizeof(buf_normal), "<red>hello</red> world");
  crsprintf_stateful(buf_stateful, sizeof(buf_stateful), state, "<red>hello</red> world");

  ASSERT_STR_EQ(buf_stateful, buf_normal);
  crprintf_state_free(state);
  crprintf_set_color(true);
}

TEST(state_carryover_unclosed_tag) {
  char buf1[256], buf2[256];
  crprintf_set_color(false);
  crprintf_state *state = crprintf_state_new();

  crsprintf_stateful(buf1, sizeof(buf1), state, "<green>hello");
  ASSERT_STR_EQ(buf1, "hello");

  crsprintf_stateful(buf2, sizeof(buf2), state, " world</>");
  ASSERT_STR_EQ(buf2, " world");

  crprintf_state_free(state);
  crprintf_set_color(true);
}

TEST(state_carryover_nested_tags) {
  crprintf_set_color(false);
  crprintf_state *state = crprintf_state_new();
  char buf[256];

  crsprintf_stateful(buf, sizeof(buf), state, "<bold><red>text");
  ASSERT_STR_EQ(buf, "text");

  crprintf_state *before = crprintf_state_clone(state);
  crsprintf_stateful(buf, sizeof(buf), state, "</>");
  ASSERT_STR_EQ(buf, "");

  ASSERT_EQ(crprintf_state_eq(state, before), false);
  crsprintf_stateful(buf, sizeof(buf), state, "</>");

  crprintf_state *empty = crprintf_state_new();
  ASSERT_EQ(crprintf_state_eq(state, empty), true);

  crprintf_state_free(state);
  crprintf_state_free(before);
  crprintf_state_free(empty);
  crprintf_set_color(true);
}

TEST(state_reset_clears) {
  crprintf_set_color(false);
  crprintf_state *state = crprintf_state_new();
  char buf[256];

  crsprintf_stateful(buf, sizeof(buf), state, "<bold><red>styled");
  crsprintf_stateful(buf, sizeof(buf), state, "<reset/>clean");
  ASSERT_STR_EQ(buf, "clean");

  crprintf_state *empty = crprintf_state_new();
  ASSERT_EQ(crprintf_state_eq(state, empty), true);

  crprintf_state_free(state);
  crprintf_state_free(empty);
  crprintf_set_color(true);
}

TEST(state_clone_independent) {
  crprintf_set_color(false);
  crprintf_state *state = crprintf_state_new();
  char buf[256];

  crsprintf_stateful(buf, sizeof(buf), state, "<green>open");
  crprintf_state *cloned = crprintf_state_clone(state);

  crsprintf_stateful(buf, sizeof(buf), state, "<bold>more");
  ASSERT_EQ(crprintf_state_eq(state, cloned), false);

  crprintf_state_free(state);
  crprintf_state_free(cloned);
  crprintf_set_color(true);
}

TEST(state_eq_null_handling) {
  ASSERT_EQ(crprintf_state_eq(NULL, NULL), true);

  crprintf_state *s = crprintf_state_new();
  ASSERT_EQ(crprintf_state_eq(s, NULL), false);
  ASSERT_EQ(crprintf_state_eq(NULL, s), false);
  crprintf_state_free(s);
}

TEST(compiled_basic) {
  char buf[256];
  crprintf_set_color(false);

  crprintf_compiled *prog = crprintf_compile("hello %s");
  crsprintf_compiled(buf, sizeof(buf), NULL, prog, "world");
  ASSERT_STR_EQ(buf, "hello world");

  crprintf_compiled_free(prog);
  crprintf_set_color(true);
}

TEST(recompile_identity) {
  const char *fmt = "<red>hello</red> world";
  crprintf_compiled *prog = crprintf_recompile(NULL, fmt);
  crprintf_compiled *same = crprintf_recompile(prog, fmt);

  ASSERT_EQ(prog == same, true);
  crprintf_compiled_free(same);
}

TEST(recompile_tail_edit) {
  char buf[256];
  crprintf_set_color(false);

  crprintf_compiled *prog = crprintf_recompile(NULL, "<red>hello</red> world");
  crsprintf_compiled(buf, sizeof(buf), NULL, prog);
  ASSERT_STR_EQ(buf, "hello world");

  prog = crprintf_recompile(prog, "<red>hello</red> world!");
  crsprintf_compiled(buf, sizeof(buf), NULL, prog);
  ASSERT_STR_EQ(buf, "hello world!");

  crprintf_compiled_free(prog);
  crprintf_set_color(true);
}

TEST(recompile_middle_edit) {
  char buf[256];
  crprintf_set_color(false);

  crprintf_compiled *prog = crprintf_recompile(NULL, "<red>hello</red> <blue>world</blue>");
  crsprintf_compiled(buf, sizeof(buf), NULL, prog);
  ASSERT_STR_EQ(buf, "hello world");

  prog = crprintf_recompile(prog, "<red>hello</red> <blue>earth</blue>");
  crsprintf_compiled(buf, sizeof(buf), NULL, prog);
  ASSERT_STR_EQ(buf, "hello earth");

  crprintf_compiled_free(prog);
  crprintf_set_color(true);
}

TEST(recompile_from_null) {
  char buf[256];
  crprintf_set_color(false);

  crprintf_compiled *prog = crprintf_recompile(NULL, "plain text");
  crsprintf_compiled(buf, sizeof(buf), NULL, prog);
  ASSERT_STR_EQ(buf, "plain text");

  crprintf_compiled_free(prog);
  crprintf_set_color(true);
}

TEST(compiled_with_state) {
  char buf[256];
  crprintf_set_color(false);
  crprintf_state *state = crprintf_state_new();

  crprintf_compiled *prog1 = crprintf_compile("<green>open");
  crsprintf_compiled(buf, sizeof(buf), state, prog1);
  ASSERT_STR_EQ(buf, "open");

  crprintf_compiled *prog2 = crprintf_compile(" still green</>");
  crsprintf_compiled(buf, sizeof(buf), state, prog2);
  ASSERT_STR_EQ(buf, " still green");

  crprintf_state *empty = crprintf_state_new();
  ASSERT_EQ(crprintf_state_eq(state, empty), true);

  crprintf_state_free(state);
  crprintf_state_free(empty);
  crprintf_compiled_free(prog1);
  crprintf_compiled_free(prog2);
  crprintf_set_color(true);
}

int main(void) {
  printf("=== crprintf tests ===\n\n");
  
  RUN_TEST(basic_string);
  RUN_TEST(format_specifiers);
  RUN_TEST(color_tags_stripped_in_no_color_mode);
  RUN_TEST(padding_right);
  RUN_TEST(padding_left);
  RUN_TEST(newlines);
  RUN_TEST(spaces);
  RUN_TEST(escapes);
  RUN_TEST(extra_styles);
  RUN_TEST(reset);
  RUN_TEST(variables);
  RUN_TEST(buffer_overflow);

  printf("\n--- stateful ---\n");
  RUN_TEST(state_new_is_clean);
  RUN_TEST(stateful_matches_non_stateful);
  RUN_TEST(state_carryover_unclosed_tag);
  RUN_TEST(state_carryover_nested_tags);
  RUN_TEST(state_reset_clears);
  RUN_TEST(state_clone_independent);
  RUN_TEST(state_eq_null_handling);

  printf("\n--- compiled / recompile ---\n");
  RUN_TEST(compiled_basic);
  RUN_TEST(recompile_identity);
  RUN_TEST(recompile_tail_edit);
  RUN_TEST(recompile_middle_edit);
  RUN_TEST(recompile_from_null);
  RUN_TEST(compiled_with_state);
  
  printf("\n=== Results: %d/%d tests passed ===\n", pass_count, test_count);
  
  return (pass_count == test_count) ? 0 : 1;
}
