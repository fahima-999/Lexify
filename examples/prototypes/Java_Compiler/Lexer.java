import java.util.*;
public class Lexer {
 private final String src; private int pos=0,line=1;
 public Lexer(String src){this.src=src;}
 public List<Token> scan(){ List<Token> out=new ArrayList<>(); while(pos<src.length()){
 char c=src.charAt(pos); if(c=='\n'){line++;pos++;continue;} if(Character.isWhitespace(c)){pos++;continue;}
 if(c=='/' && pos+1<src.length() && src.charAt(pos+1)=='/'){while(pos<src.length()&&src.charAt(pos)!='\n')pos++;continue;}
 if(Character.isDigit(c)){int s=pos;while(pos<src.length()&&Character.isDigit(src.charAt(pos)))pos++;out.add(new Token(TokenType.NUMBER,src.substring(s,pos),line));continue;}
 if(Character.isLetter(c)||c=='_'){int s=pos;while(pos<src.length()&&(Character.isLetterOrDigit(src.charAt(pos))||src.charAt(pos)=='_'))pos++;String word=src.substring(s,pos);out.add(new Token(word.equals("int")?TokenType.INT:word.equals("print")?TokenType.PRINT:TokenType.IDENT,word,line));continue;}
 pos++;TokenType t=switch(c){case '+'->TokenType.PLUS;case '-'->TokenType.MINUS;case '*'->TokenType.STAR;case '/'->TokenType.SLASH;case '='->TokenType.ASSIGN;case ';'->TokenType.SEMI;case '('->TokenType.LPAREN;case ')'->TokenType.RPAREN;default->throw new IllegalArgumentException("Unexpected character '"+c+"' at line "+line);};out.add(new Token(t,Character.toString(c),line));
 }out.add(new Token(TokenType.EOF,"",line));return out;}
}
