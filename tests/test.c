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
  RUN_TEST(reset);
  RUN_TEST(variables);
  RUN_TEST(buffer_overflow);
  
  printf("\n=== Results: %d/%d tests passed ===\n", pass_count, test_count);
  
  return (pass_count == test_count) ? 0 : 1;
}
