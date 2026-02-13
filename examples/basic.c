#include "crprintf.h"

#include <stdbool.h>
#include <crprintf.h>
#include <string.h>

int main(void) {
  // basic colors
  crprintf("<red>Red text</red> and back to normal\n");
  crprintf("<green>Green</green>, <blue>Blue</blue>, <yellow>Yellow</yellow>\n");
  
  // bright colors
  crprintf("<bright_red>Bright Red</bright_red> vs <red>Normal Red</red>\n");
  
  // background colors
  crprintf("<bg_red>Red background</bg_red> with text\n");
  crprintf("<bg_blue><white>Blue bg with white text</white></bg_blue>\n");
  
  // styles
  crprintf("<bold>Bold text</bold> and <dim>dim text</dim>\n");
  crprintf("<ul>Underlined</ul> text\n");
  
  // combined styles
  crprintf("<bold_red>Bold red</bold_red> or <bold+blue>bold blue</bold+blue>\n");
  crprintf("<dim_cyan>Dim cyan</dim_cyan>\n");
  
  // hex colors
  crprintf("<#ff8800>Orange hex color</#ff8800>\n");
  crprintf("<#0f0>Bright green shorthand</#0f0>\n");
  crprintf("<bg_#ff00ff>Magenta background</bg_#ff00ff>\n");
  
  // variables
  crprintf_var("error_style", "bold_red");
  crprintf_var("info_style", "cyan");
  
  // padding
  crprintf("Left aligned:  <pad=20><$info_style>command</></pad> description\n");
  crprintf("Right aligned: <rpad=20><$error_style>error</></rpad> message\n");
  
  crprintf(
    "<let status=bold+green><$status>Success!</$status></let>\n"
    "{status} is the variable value\n"
  );
  
  // format specifiers
  crprintf("<bold>Number: %d, String: %s</bold>\n", 42, "hello");
  crprintf("<green>Hex: 0x%x, Float: %.2f</green>\n", 255, 3.14159);
  
  // reset
  crprintf("<red>red <reset/>back to normal immediately\n");
  
  // escapes
  crprintf("Literal <<: less than, >>: greater than\n");
  crprintf("Literal %%: percent sign\n");
  
  // spaces and breaks
  crprintf("Some text<space=10/>spaced out<br/>New line<br=2/>Two new lines\n");
  
  return 0;
}
