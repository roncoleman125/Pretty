package pretty

import java.io.FileInputStream
import org.antlr.v4.runtime.ANTLRInputStream
import org.antlr.v4.runtime.CommonTokenStream
import org.antlr.v4.runtime.tree.ParseTreeListener
import org.antlr.v4.runtime.tree.ErrorNode
import org.antlr.v4.runtime.tree.ParseTree
import org.antlr.v4.runtime.tree.TerminalNode
import cgrammar.CLexer
import cgrammar.CParser
import org.antlr.v4.runtime.ParserRuleContext
import scala.collection.mutable.HashMap

object Cyclomatic {
  def main(args: Array[String]): Unit = {
    println(em(args(0)))
  }
  
  def em(inputFile: String): Int = {
    val in = new FileInputStream(inputFile)
    
    val input = new ANTLRInputStream(in)

    val lexer = new CLexer(input)
    
    val tokens = new CommonTokenStream(lexer)
    
    val parser = new CParser(tokens)

    val listener = new MyParserListener

    parser.addParseListener(listener)

    parser.functionDefinition

    val m = listener.decisions.foldLeft(1) { (sum, keyValue) =>
      val (token, count) = keyValue

      sum + count
    }

    m
  }
}

class MyParserListener extends ParseTreeListener {
  val decisions = new HashMap[String,Int]()
  
  def enterEveryRule(arg: ParserRuleContext): Unit = {}
  
  def exitEveryRule(arg: ParserRuleContext): Unit = {}
  
  def visitErrorNode(arg: ErrorNode): Unit = {}
  
  def visitTerminal(arg: TerminalNode): Unit = {
    val token = arg.getText
    token match {
      case "if" | "else" | "for" | "while" | "case" | "||" | "&&" =>
        val count = decisions.getOrElse(token, 0)
        decisions(token) = count + 1
      case _ =>
    }
//    println(token)
  }
}