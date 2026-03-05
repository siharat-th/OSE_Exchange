// reset
#define COLOR_RESET        "\033[0m"    // Reset all attributes

// regular foreground colours
#define FG_BLACK           "\033[30m"   // Black
#define FG_RED             "\033[31m"   // Red
#define FG_GREEN           "\033[32m"   // Green
#define FG_YELLOW          "\033[33m"   // Yellow
#define FG_BLUE            "\033[34m"   // Blue
#define FG_MAGENTA         "\033[35m"   // Magenta
#define FG_CYAN            "\033[36m"   // Cyan
#define FG_WHITE           "\033[37m"   // White

// bright (bold) foreground colours
#define FG_BRIGHT_BLACK    "\033[90m"   // Bright Black (Gray)
#define FG_BRIGHT_RED      "\033[91m"   // Bright Red
#define FG_BRIGHT_GREEN    "\033[92m"   // Bright Green
#define FG_BRIGHT_YELLOW   "\033[93m"   // Bright Yellow
#define FG_BRIGHT_BLUE     "\033[94m"   // Bright Blue
#define FG_BRIGHT_MAGENTA  "\033[95m"   // Bright Magenta
#define FG_BRIGHT_CYAN     "\033[96m"   // Bright Cyan
#define FG_BRIGHT_WHITE    "\033[97m"   // Bright White

// regular background colours
#define BG_BLACK           "\033[40m"   // Black background
#define BG_RED             "\033[41m"   // Red background
#define BG_GREEN           "\033[42m"   // Green background
#define BG_YELLOW          "\033[43m"   // Yellow background
#define BG_BLUE            "\033[44m"   // Blue background
#define BG_MAGENTA         "\033[45m"   // Magenta background
#define BG_CYAN            "\033[46m"   // Cyan background
#define BG_WHITE           "\033[47m"   // White background

// bright background colours
#define BG_BRIGHT_BLACK    "\033[100m"  // Bright Black background (Gray)
#define BG_BRIGHT_RED      "\033[101m"  // Bright Red background
#define BG_BRIGHT_GREEN    "\033[102m"  // Bright Green background
#define BG_BRIGHT_YELLOW   "\033[103m"  // Bright Yellow background
#define BG_BRIGHT_BLUE     "\033[104m"  // Bright Blue background
#define BG_BRIGHT_MAGENTA  "\033[105m"  // Bright Magenta background
#define BG_BRIGHT_CYAN     "\033[106m"  // Bright Cyan background
#define BG_BRIGHT_WHITE    "\033[107m"  // Bright White background

// 256-colour mode helpers (foreground/background)
// Usage: std::cout << FG_256(202) << "hello" << COLOR_RESET;
#define FG_256(n)          "\033[38;5;" #n "m"
#define BG_256(n)          "\033[48;5;" #n "m"

// 24-bit “true colour” mode helpers
// Usage: std::cout << FG_RGB(255,128,0) << "hello" << COLOR_RESET;
#define FG_RGB(r,g,b)      "\033[38;2;" #r ";" #g ";" #b "m"
#define BG_RGB(r,g,b)      "\033[48;2;" #r ";" #g ";" #b "m"
