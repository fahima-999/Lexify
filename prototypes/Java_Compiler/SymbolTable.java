import java.util.*;
public class SymbolTable {
 public record Symbol(Ast.Type type,Object value) {}
 private final Map<String,Symbol> values=new LinkedHashMap<>();
 private static Object coerce(Ast.Type type,Object v){
  if(type==Ast.Type.DOUBLE&&v instanceof Number n)return n.doubleValue();
  if(type==Ast.Type.INT&&v instanceof Integer)return v;
  throw new IllegalArgumentException("Cannot assign "+(v instanceof Double?"double":"value")+" to "+type+" variable");
 }
 private Symbol lookup(String n){Symbol s=values.get(n);if(s==null)throw new IllegalArgumentException("Undefined variable: "+n);return s;}
 public boolean contains(String n){return values.containsKey(n);}
 public void declare(String n,Ast.Type type,Object v){if(contains(n))throw new IllegalArgumentException("Variable already declared: "+n);values.put(n,new Symbol(type,coerce(type,v)));}
 public void set(String n,Object v){Symbol s=lookup(n);values.put(n,new Symbol(s.type(),coerce(s.type(),v)));}
 public Object get(String n){return lookup(n).value();}
 public Ast.Type typeOf(String n){return lookup(n).type();}
 public Map<String,Symbol> snapshot(){return Collections.unmodifiableMap(values);}
}
