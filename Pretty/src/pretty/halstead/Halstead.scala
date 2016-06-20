package pretty.halstead

import java.io.FileInputStream
import org.antlr.v4.runtime.ANTLRInputStream
import org.antlr.v4.runtime.CommonTokenStream
import org.antlr.v4.runtime.tree.ParseTreeListener
import org.antlr.v4.runtime.tree.ErrorNode
import org.antlr.v4.runtime.tree.ParseTree
import org.antlr.v4.runtime.tree.TerminalNode
import cg4.CLexer
import cg4.CParser
import org.antlr.v4.runtime.ParserRuleContext
import scala.collection.mutable.HashMap
import scala.io.Source

object Halstead {
  val LOG2 = Math.log(2)
  
  def main(args: Array[String]): Unit = {
    val in = new FileInputStream(args(0))
    
    val input = new ANTLRInputStream(in)

    val lexer = new CLexer(input)
    
    val tokens = new CommonTokenStream(lexer)
    
    val parser = new CParser(tokens)

    val listener = new HalsteadParserListener

    parser.addParseListener(listener)

    parser.functionDefinition
   
    println("%s\t%s".format("OPERATOR","N"))
    listener.operators.foreach { e =>
      val (token, freq) = e
      println("%s\t\t%d".format(token,freq))
    }
    
    println("\n%s\t\t%s".format("OPERAND","N"))
    listener.operands.foreach { e =>
      val (token, freq) = e
      println("%s\t\t%d".format(token,freq))
    }
    
    // Compute program length
    val N1 = listener.operators.values.sum
    val N2 = listener.operands.values.sum
    val N = N1 + N2
    
    // Compute program vocabulary
    val n1 = listener.operators.keySet.size
    val n2 = listener.operands.keySet.size
    val n = n1 + n2
    
    // Compute volume

    val V = N * Math.log(n) / LOG2
    
    // Compute difficulty
    val D = n1 * N2 / (2.toDouble * n2)
    
    // Compute effort
    val E = D * V
    
    // Compute lines and characters
    val lines = Source.fromFile(args(0)).getLines.toList
    val numLines = lines.length
    val numChars = lines.foldLeft(0) { (sum,line) => sum + line.length() }
    
    val numUniqueTokens = listener.tokens.keySet.size

    val hTokens = entropy(listener.tokens)
    val hChars = entropy(listener.chars)
    println("%-10s %6s %6s %6s %6s %6s %6s %6s %6s %6s %6s".
        format("File","Lines","Chars","N","n","V","D","E","H:tok","Tokens","H:char"))
    println("%-10s %6d %6d %6d %6d %6.1f %6.1f %6.1f %6.1f %6d %6.1f".format(basename(args(0)),numLines,numChars,N,n,V,D,E,hTokens,numUniqueTokens,hChars))
  }
  
  def entropy(map: HashMap[String,Int]): Double = {
    val total = map.values.sum
    val h = map.foldLeft(0.0) { (sum,itemCount) =>
      val (item,count) = itemCount
      
      val p = count / total.toDouble
      sum - p * Math.log(p) / LOG2
    }   
    
    h
  }
  
  def basename(s: String): String = {
    val j = s.lastIndexOf('/')
    if(j == -1)
      s
    else
      s.substring(j+1)
  }
}

class HalsteadParserListener extends ParseTreeListener {
  val operators = new HashMap[String,Int]()
  val operands = new HashMap[String,Int]()
  val tokens = new HashMap[String,Int]()
  val chars = new HashMap[String,Int]()
  var isUnaryOpContext = false
  var isDeclarContext = false
  var isExprContext = false
  var compoundSttDepth = 0
  var isPostfixContext = false
  
  // C keywords
  val kws = List(
      "auto",
      "break",
      "bool",
      "case",
      "char",
      "const",
      "continue",
      "default",
      "do",
      "double",
      "else",
      "enum",
      "extern",
      "float",
      "for",
      "goto",
      "if",
      "int",
      "long",
      "register",
      "return",
      "short",
      "signed",
      "sizeof",
      "static",
      "struct",
      "switch",
      "typedef",
      "union",
      "unsigned",
      "void",
      "volatile",
      "while",
      "true",
      "false",
      // Compiler directives
      "define",
      "include",
      "ifdef",
      "ifndef",
      "endif",
      "elif",
      "undef",
      "warning",
      "line",
      "pragma"
  )
  val keyWords = HashMap[String,String]()
  kws.foreach { kw => keyWords(kw) = kw}
  
  def enterEveryRule(arg: ParserRuleContext): Unit = {
    val const = arg.isInstanceOf[CParser.ConstantExpressionContext]
    if(const) {
      println("constant: "+arg)
    }
    
    arg match {
      case node: CParser.UnaryOperatorContext =>
        isUnaryOpContext = true
        println("GOT UNARY!")
        
      case node: CParser.ExpressionContext =>
        isExprContext = true
        println("GOT EXPR!")
        
      case node: CParser.InitDeclaratorContext =>
        isDeclarContext = true
        println("GOT INIT DECLAR!")
        
//      case node: CParser.DeclarationContext =>
//        isDeclarContext = true
//        println("GOT DECLAR!")
        
      case node: CParser.CompoundStatementContext =>
        compoundSttDepth += 1
        println("GOT COMPOUND!")
        
      case node: CParser.PostfixExpressionContext =>
        isPostfixContext = true
        println("GOT POSTFIX!")
        
      case _ =>
    }
  }
  
  def exitEveryRule(arg: ParserRuleContext): Unit = {}
  
  def visitErrorNode(arg: ErrorNode): Unit = {}
  
  def visitTerminal(arg: TerminalNode): Unit = {  
    // TODO: arrays
    // TODO: nested expressions
    // TODO: multiple unary expressions, eg, i++ + -j + *--p
    val token = arg.getText
    
    println("token = "+token)
    
    val numTokens = tokens.getOrElse(token, 0)
    tokens(token) = numTokens + 1
    
    val numChars = chars
    token.foreach { c =>
      val s = c.toString
      val numChars = chars.getOrElse(s, 0)
      chars(s) = numChars + 1
    }
    
    // Process only with context of a function or procedure
    if(compoundSttDepth == 0)
      return
    
    val parent = arg.getParent
    val numChildren = parent.getChildCount
    val parentText = parent.getText

    token match {
      case token if isNumber(token) || isString(token) =>
        val count = operands.getOrElse(token,0)
        operands(token) = count + 1
      
      case token if isIdentifier(token) && isExprContext =>
        val count = operands.getOrElse(token,0)
        operands(token) = count + 1
        isExprContext = false
      
      case token if isFunction(token) && isPostfixContext =>
        val count = operands.getOrElse(token,0)
        operands(token) = count + 1
        isPostfixContext = false
          
      case ("*" | "-" | "--" | "++" ) if isUnaryOpContext =>
          val key = "UNARY" + token
          val states = parent.toStringTree()
          val op = parent.getChild(numChildren-1)
          val count = operators.getOrElse(key,0)
          operators(key) = count + 1         
          isUnaryOpContext = false    
          
      case "++" | "--" if isPostfixContext =>
          val key = token + "UNARY"
          val op = parent.getChild(numChildren-1)
          val count = operators.getOrElse(key,0)
          operators(key) = count + 1 
          isPostfixContext = true
          
      case "=" | "*" | "+" | "/" | "<" | ">" | "." | "%" | "&" | "|" | "!" | "^" | "~" |
           "<=" | ">=" | "==" | "-=" | "+=" | "*=" | "/=" | "%=" |
           "||" | "&&" | "->" | "<<" | ">>" |
           "[" | 
           "?" =>

        if(isDeclarContext) {
          val key = "INIT"
          val states = parent.toStringTree()
          val op = parent.getChild(numChildren-1)
          val operatorCount = operands.getOrElse(key,0)
          operators(key) = operatorCount + 1 
          isDeclarContext = false
        }
        else {         
          val op = parent.getChild(numChildren-1).getText.trim
          val operatorCount = operators.getOrElse(op,0)
          operators(op) = operatorCount + 1
        }
        
      case _ =>
    }
  }
  
  def isNumber(s: String) = s(0).isDigit
  
  def isString(s: String) = s.startsWith("\"")
  
  def isIdentifier(s: String): Boolean = {
    if(/*!isExprContext ||*/ s.length == 0)
      false
    else
      !keyWords.keys.exists(_ == s) && (s(0).isLetter || s(0) == '_')
//      kws.find { word => word == s } match {
//        case Some(w) => false
//        case None => s(0).isLetter || s(0) == '_'
//      }
  }
  
  def isFunction(s: String): Boolean = {
    isIdentifier(s)
  }
}