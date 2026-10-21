import java.util.*;
public class CodeGenerator {public List<String> generate(Ast.Program p){return new TacGenerator().generate(p);}}
