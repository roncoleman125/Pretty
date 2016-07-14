package pretty.util

import java.io.FileInputStream

import org.antlr.v4.runtime.ANTLRInputStream
import org.antlr.v4.runtime.CommonTokenStream
import org.antlr.v4.runtime.ParserRuleContext
import org.antlr.v4.runtime.tree.ErrorNode
import org.antlr.v4.runtime.tree.ParseTreeListener
import org.antlr.v4.runtime.tree.TerminalNode

import cgrammar.CLexer
import cgrammar.CParser
import pretty.readability.HalsteadParserListener

object Tag {
  def main(args: Array[String]): Unit = {
    val in = new FileInputStream(args(0))
    
    // Get tne ANTLR parser loaded
    val input = new ANTLRInputStream(in)

    val lexer = new CLexer(input)
    
    val tokens = new CommonTokenStream(lexer)
    
    val parser = new CParser(tokens)

    val listener = new TagParserListener(tokens)
    
    parser.addParseListener(listener)
    
    parser.compilationUnit
  }
}

class TagParserListener(tokenStream: CommonTokenStream) extends ParseTreeListener   {
  def enterEveryRule(arg: ParserRuleContext): Unit = {  
    arg match {
      case node: CParser.FunctionDefinitionContext =>
        val lineno = node.getStart.getLine
        println(lineno)
        
      case _ =>
    }
  }
  
  def exitEveryRule(arg: ParserRuleContext): Unit = {}
  
  def visitErrorNode(arg: ErrorNode): Unit = {}
  
  def visitTerminal(arg: TerminalNode): Unit = {
    

  }
}