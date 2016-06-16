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

object Halstead {
  def main(args: Array[String]): Unit = {
    val in = new FileInputStream(args(0))
    
    val input = new ANTLRInputStream(in)

    val lexer = new CLexer(input)
    
    val tokens = new CommonTokenStream(lexer)
    
    val parser = new CParser(tokens)

    val listener = new MyParserListener

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
  }
}

class MyParserListener extends ParseTreeListener {
  val decisions = new HashMap[String,Int]()
  val operators = new HashMap[String,Int]()
  val operands = new HashMap[String,Int]()
  var unaryOp = false
  var declar = false
  var expr = false
  var func = false
  var postfix = false
  val keyWords = List("for", "char", "int")
  
  def enterEveryRule(arg: ParserRuleContext): Unit = {
    val const = arg.isInstanceOf[CParser.ConstantExpressionContext]
    if(const) {
      println("constant: "+arg)
    }
    
    arg match {
      case node: CParser.UnaryOperatorContext =>
        unaryOp = true
        println("GOT UNARY!")
        
      case node: CParser.ExpressionContext =>
        expr = true
        println("GOT EXPR!")
        
      case node: CParser.InitDeclaratorContext =>
        declar = true
        println("GOT DECLAR!")
        
      case node: CParser.CompoundStatementContext =>
        func = true
        println("GOT COMPOUND!")
        
      case node: CParser.PostfixExpressionContext =>
        postfix = true
        println("GOT POSTFIX!")
        
      case _ =>
    }
//    
//    if(arg.isInstanceOf[CParser.UnaryOperatorContext]) {
//      unaryOp = true
//      println("got unary!")
//    }
//    else if(arg.isInstanceOf[CParser.InitDeclaratorContext]) {
//      declar = true
//      println("got declar!")
//    }
//    else if(arg.isInstanceOf[CParser.ExpressionContext])  {
//      expr = true
//      println("got expr!")
//    }
//    
//    if(arg.isInstanceOf[CParser.CompoundStatementContext])  {
//      func = true
//      println("GOT FUNC!")
//    }
//    
//    if(arg.isInstanceOf[CParser.PostfixExpressionContext])  {
//      postfix = true
//      println("GOT POSTFIX!")
//    }    
  }
  
  def exitEveryRule(arg: ParserRuleContext): Unit = {}
  
  def visitErrorNode(arg: ErrorNode): Unit = {}
  
  def visitTerminal(arg: TerminalNode): Unit = {     
    val token = arg.getText
    
    println("token = "+token)
    
    if(!func)
      return
    
//    if(isNumber(token) || isString(token)) {
//      val count = operands.getOrElse(token,0)
//      operands(token) = count + 1
//    }
//    else if(isIdentifier(token)) {
//      val count = operands.getOrElse(token,0)
//      operands(token) = count + 1
//      expr = false
//    }
//    else if(isFunction(token) && postfix) {
//      val count = operands.getOrElse(token,0)
//      operands(token) = count + 1
//      postfix = false
//    }

    val parent = arg.getParent

    token match {
      case token if isNumber(token) || isString(token) =>
        val count = operands.getOrElse(token,0)
        operands(token) = count + 1
      
      case token if isIdentifier(token) =>
        val count = operands.getOrElse(token,0)
        operands(token) = count + 1
        expr = false
      
      case token if isFunction(token) && postfix =>
        val count = operands.getOrElse(token,0)
        operands(token) = count + 1
        postfix = false
        
      case ("*" | "-" | "--" | "++" ) if unaryOp =>
          val key = "UNARY" + token
          val states = parent.toStringTree()
          val op = parent.getChild(1)
          val count = operators.getOrElse(key,0)
          operators(key) = count + 1         
          unaryOp = false    
          
      case "++" | "--" if postfix =>
          val key = token + "UNARY"
          val states = parent.toStringTree()
          val op = parent.getChild(1)
          val count = operators.getOrElse(key,0)
          operators(key) = count + 1 
          postfix = true
          
      case "=" | "+=" | "*" | "==" | "+" | "||" | "&&" | "." | "->" | "%" | "/" | "<<" | ">>" =>
        val numChildren = parent.getChildCount
        if(declar) {
          val states = parent.toStringTree()
          val op = parent.getChild(1)
          val operatorCount = operands.getOrElse("INIT",0)
          operators("INIT") = operatorCount + 1 
          declar = false
        }
        else if( numChildren > 1) {         
          val op = parent.getChild(1).getText.trim
          val m = parent.getChild(1).getChildCount
          val operatorCount = operators.getOrElse(op,0)
          operators(op) = operatorCount + 1
        }
        val parentText = parent.getText
        val count = decisions.getOrElse(token, 0)
        decisions(token) = count + 1
        
      case _ =>
    }
  }
  
  def isNumber(s: String) = s(0).isDigit
  
  def isString(s: String) = s.startsWith("\"")
  
  def isIdentifier(s: String): Boolean = {
    if(!expr)
      false
    else
      keyWords.find { word => word == s } match {
        case Some(w) => false
        case None => s(0).isLetter
      }
  }
  
  def isFunction(s: String): Boolean = {
    keyWords.find { word => word == s } match {
      case Some(word) => false
      case None => s(0).isLetter 
    }
  }
}