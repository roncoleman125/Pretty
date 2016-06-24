/*
 * Copyright (c) Pretty Contributors
 * See CONTRIBUTORS.TXT for a full list of copyright holders.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Scaly Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
package pretty.halstead

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
import scala.io.Source
import pretty.util.Helper

object Halstead {
  val LOG2 = Math.log(2)
  
  def main(args: Array[String]): Unit = {
    args.foreach {
      path => process(path)
    }
  }
  
  def process(path: String): Unit = {
    val in = new FileInputStream(path)
    
    // Get tne ANTLR parser loaded
    val input = new ANTLRInputStream(in)

    val lexer = new CLexer(input)
    
    val tokens = new CommonTokenStream(lexer)
    
    val parser = new CParser(tokens)

    val listener = new HalsteadParserListener

    parser.addParseListener(listener)

    // Process only function definitions
    parser.functionDefinition
   
    Helper.log("%s\t%s".format("OPERATOR","N"))
    listener.operators.foreach { e =>
      val (token, freq) = e
      Helper.log("%s\t\t%d".format(token,freq))
    }
    
    Helper.log("\n%s\t\t%s".format("OPERAND","N"))
    listener.operands.foreach { e =>
      val (token, freq) = e
      Helper.log("%s\t\t%d".format(token,freq))
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
    val lines = Source.fromFile(path).getLines.toList
    val numLines = lines.length
    val numChars = lines.foldLeft(0) { (sum,line) => sum + line.length() }
    
    val numUniqueTokens = listener.tokens.keySet.size

    val hTokens = entropy(listener.tokens)
    val hChars = entropy(listener.chars)
    
    val z = 8.87 - 0.033*V + 0.40*numLines - 1.5*hTokens
    val logit = 1.0 / (1 + Math.exp(-z))
    
    println("%-10s %6s %6s %6s %6s %6s %6s %6s %6s %6s %6s %6s %6s".
        format("File","Lines","Chars","N","n","V","D","E","H:tok","Tokens","H:char","z","logit"))
    println("%-10s %6d %6d %6d %6d %6.1f %6.1f %6.1f %6.1f %6d %6.1f %6.2f %6.4f".
        format(basename(path),numLines,numChars,N,n,V,D,E,hTokens,numUniqueTokens,hChars,z,logit))
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
      Helper.log("constant: "+arg)
    }
    
    arg match {
      case node: CParser.UnaryOperatorContext =>
        isUnaryOpContext = true
        Helper.log("GOT UNARY!")
        
      case node: CParser.ExpressionContext =>
        isExprContext = true
        Helper.log("GOT EXPR!")
        
      case node: CParser.InitDeclaratorContext =>
        isDeclarContext = true
        Helper.log("GOT INIT DECLAR!")
        
//      case node: CParser.DeclarationContext =>
//        isDeclarContext = true
//        Helper.log("GOT DECLAR!")
        
      case node: CParser.CompoundStatementContext =>
        compoundSttDepth += 1
        Helper.log("GOT COMPOUND!")
        
      case node: CParser.PostfixExpressionContext =>
        isPostfixContext = true
        Helper.log("GOT POSTFIX!")
        
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
    
    Helper.log("token = "+token)
    
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

class DudParserListener extends ParseTreeListener {
  def enterEveryRule(arg: ParserRuleContext): Unit = {
  }
  def exitEveryRule(arg: ParserRuleContext): Unit = {}
  
  def visitErrorNode(arg: ErrorNode): Unit = {}
  
  def visitTerminal(arg: TerminalNode): Unit = {}
}