import java.util.*;
public class Lexer {
 private static final Map<String,TokenType> KEYWORDS=Map.ofEntries(
  Map.entry("int",TokenType.INT),Map.entry("double",TokenType.DOUBLE),Map.entry("return",TokenType.RETURN),
  Map.entry("class",TokenType.CLASS),Map.entry("public",TokenType.PUBLIC),Map.entry("static",TokenType.STATIC),
  Map.entry("void",TokenType.VOID),Map.entry("main",TokenType.MAIN),Map.entry("String",TokenType.STRING),
  Map.entry("System",TokenType.SYSTEM),Map.entry("out",TokenType.OUT),Map.entry("println",TokenType.PRINTLN));
 private final String src; private int pos=0,line=1;
 public Lexer(String src){this.src=src;}
 public List<Token> scan(){ List<Token> out=new ArrayList<>(); while(pos<src.length()){
  char c=src.charAt(pos); if(c=='\n'){line++;pos++;continue;} if(Character.isWhitespace(c)){pos++;continue;}
  // line comment
  if(c=='/' && pos+1<src.length() && src.charAt(pos+1)=='/'){while(pos<src.length()&&src.charAt(pos)!='\n')pos++;continue;}
  // block comment
  if(c=='/' && pos+1<src.length() && src.charAt(pos+1)=='*'){
   int start=line;pos+=2;
   while(pos+1<src.length()&&!(src.charAt(pos)=='*'&&src.charAt(pos+1)=='/')){if(src.charAt(pos)=='\n')line++;pos++;}
   if(pos+1>=src.length())throw new IllegalArgumentException("Unclosed comment starting at line "+start);
   pos+=2;continue;
  }
  // number: digits with an optional fractional part
  if(Character.isDigit(c)){int s=pos;while(pos<src.length()&&Character.isDigit(src.charAt(pos)))pos++;
   if(pos<src.length()&&src.charAt(pos)=='.'){pos++;while(pos<src.length()&&Character.isDigit(src.charAt(pos)))pos++;}
   out.add(new Token(TokenType.NUMBER,src.substring(s,pos),line));continue;}
  // identifier / keyword
  if(Character.isLetter(c)||c=='_'){int s=pos;while(pos<src.length()&&(Character.isLetterOrDigit(src.charAt(pos))||src.charAt(pos)=='_'))pos++;
   String word=src.substring(s,pos);out.add(new Token(KEYWORDS.getOrDefault(word,TokenType.IDENT),word,line));continue;}
  pos++;
  TokenType t=switch(c){case '+'->TokenType.PLUS;case '-'->TokenType.MINUS;case '*'->TokenType.STAR;case '/'->TokenType.SLASH;case '='->TokenType.ASSIGN;case ';'->TokenType.SEMI;
   case '('->TokenType.LPAREN;case ')'->TokenType.RPAREN;case '{'->TokenType.LBRACE;case '}'->TokenType.RBRACE;case '['->TokenType.LBRACKET;case ']'->TokenType.RBRACKET;case '.'->TokenType.DOT;
   default->throw new IllegalArgumentException("Unexpected character '"+c+"' at line "+line);};
  out.add(new Token(t,Character.toString(c),line));
 }out.add(new Token(TokenType.EOF,"",line));return out;}
}
