public record Token(TokenType type, String text, int line) { public String toString() { return type+"("+text+") at line "+line; } }
