import java.util.*;
public class SymbolTable {private final Map<String,Integer> values=new LinkedHashMap<>();
 public void declare(String n,int v){if(values.containsKey(n))throw new IllegalArgumentException("Variable already declared: "+n);values.put(n,v);}
 public boolean contains(String n){return values.containsKey(n);} public int get(String n){if(!contains(n))throw new IllegalArgumentException("Undefined variable: "+n);return values.get(n);}
 public void set(String n,int v){if(!contains(n))throw new IllegalArgumentException("Undefined variable: "+n);values.put(n,v);}
 public Map<String,Integer> snapshot(){return Collections.unmodifiableMap(values);}
}
