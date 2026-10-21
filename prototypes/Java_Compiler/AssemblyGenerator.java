import java.util.*;
// Turns three-address code into educational pseudo-assembly (not real CPU or JVM instructions).
public class AssemblyGenerator {
 private static String opcode(String op){return switch(op){case "+"->"ADD";case "-"->"SUB";case "*"->"MUL";default->"DIV";};}
 public String generate(List<String> tac){
  StringBuilder out=new StringBuilder("; Educational pseudo assembly (not native machine code or JVM bytecode)\n");
  for(String line:tac){
   if(line.isBlank())continue;
   String[] w=line.trim().split("\\s+");
   if(w[0].equals("declare")&&w.length==3&&!w[1].equals("="))out.append("    DECL ").append(w[1]).append(' ').append(w[2]).append('\n');
   else if(w[0].equals("print")&&w.length==2)out.append("    PRINT ").append(w[1]).append('\n');
   else if(w[0].equals("return")&&w.length==1)out.append("    RET\n");
   else if(w.length==3&&w[1].equals("=")){
    if(w[2].startsWith("-")&&w[2].length()>1)out.append("    NEG ").append(w[0]).append(", ").append(w[2].substring(1)).append('\n');
    else out.append("    MOV ").append(w[0]).append(", ").append(w[2]).append('\n');
   }
   else if(w.length==5)out.append("    ").append(opcode(w[3])).append(' ').append(w[0]).append(", ").append(w[2]).append(", ").append(w[4]).append('\n');
   else out.append("    ; ").append(line).append('\n');
  }
  return out.toString();
 }
}
