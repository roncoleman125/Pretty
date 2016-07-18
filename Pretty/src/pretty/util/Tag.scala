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
import java.nio.file.Files
import java.io.File
import scala.util.Random
import scala.collection.mutable.ListBuffer
import java.io.ByteArrayInputStream
import scala.io.Source
import scala.collection.mutable.HashMap
import java.io.PrintWriter

object Tag {
    
  val ran = new Random(0)
  val out = new PrintWriter("/Users/roncoleman125/marist/research/Pretty/linux/meths/raw-tags.txt")
  
  def main(args: Array[String]): Unit = {       
    FileWalker.walk(args(0), ".c", action)
    out.flush
    out.close
//    val file = new File("data/linux-kernel/console.c")
//    action(file)
  }
  
  def action(file: File): Unit = {

//    // Filter out compiler directives since C.g4 cant handle them
//    val in = new FileInputStream(file)
//    
//    import org.apache.commons.io.IOUtils
//    val bytes = IOUtils.toByteArray(in)
//    
//    val path = file.getAbsolutePath
//    println(">>>>> "+path)
//
//    val filteredlines = Source.fromBytes(bytes).getLines.toList.filter { line =>
//      val pass = !line.startsWith("#")
//      
//      if(!pass)
//        println(line)
//        
//      pass
//    }.foldLeft("") { (accum, line) => accum + line + "\n" }
//    
//    println("----- "+filteredlines.length+" lines remain")
//    println(filteredlines)
//    
//    val filteredBytes = filteredlines.getBytes
//    
//    val bis = new ByteArrayInputStream(filteredBytes)

    // Compile the file looking for methods
    val input = new ANTLRInputStream(Helper.strip(file))

    val lexer = new CLexer(input)
    
    val tokens = new CommonTokenStream(lexer)
    
    val parser = new CParser(tokens)

    val listener = new TagParserListener(tokens)
    
    parser.addParseListener(listener)
    
    parser.compilationUnit
    
    out.print(file.getAbsolutePath+ " " + listener.errors + " ")
    
    listener.marks.foreach { mark =>
      out.print(mark + " ")
    }
    out.println
  }
}

class TagParserListener(tokenStream: CommonTokenStream) extends ParseTreeListener   {
  val marks = ListBuffer[Int]()
  var errors = 0
  
  def enterEveryRule(arg: ParserRuleContext): Unit = {  
    arg match {
      case node: CParser.FunctionDefinitionContext =>
        // Line numbers are 1-based, we need them 0-based
//        val start = node.getStart.getLine
//        val end = node.getStop.getLine
//        marks.append(start-1)
        
      case _ =>
    }
  }
  
  def exitEveryRule(arg: ParserRuleContext): Unit = {
    arg match {
      case node: CParser.FunctionDefinitionContext =>
        // Line numbers are 1-based, we need them 0-based.
        val start = node.getStart.getLine
        val end = node.getStop.getLine
        marks.append(start-1)
        marks.append(end-1)
        
      case _ =>
    }    
  }
  
  def visitErrorNode(arg: ErrorNode): Unit = {
    errors += 1
  }
  
  def visitTerminal(arg: TerminalNode): Unit = {
    

  }
}