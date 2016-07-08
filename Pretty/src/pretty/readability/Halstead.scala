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
package pretty.readability

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
import org.antlr.v4.runtime.Token
import scala.collection.mutable.ListBuffer

 
case class Sentence(start: Token, stop: Token)
  
object Halstead {
  val LOG2 = Math.log(2)
  
  def main(args: Array[String]): Unit = {
    
    print("%-10s %6s %6s %6s %6s %6s %6s %7s %6s %6s %6s %6s %7s ".
        format("file","loc","chars","N","n","V","D","E","H:tok","Tokens","H:char","z","phd"))
        
    println("%6s %6s %6s %6s %7s %7s %7s %6s %6s %s".
        format("sres","ncl","nbl","cc","mi","misei","mims","cc/nos","cd", "buse" ))
        
    args.foreach {
      path => process(path)
    }
  }
  
  /** Process one file */
  def process(path: String): Unit = {
    val in = new FileInputStream(path)
    
    // Get tne ANTLR parser loaded
    val input = new ANTLRInputStream(in)

    val lexer = new CLexer(input)
    
    val tokens = new CommonTokenStream(lexer)
    
    val parser = new CParser(tokens)

    val listener = new HalsteadParserListener(tokens)
    
    parser.addParseListener(listener)

    // Process only function definitions
    val tree  = parser.functionDefinition
    Helper.logger(tree.toStringTree(parser))
   
    Helper.logger("%s\t%s".format("OPERATOR","N"))
    listener.operators.foreach { e =>
      val (token, freq) = e
      Helper.logger("%s\t\t%d".format(token,freq))
    }
    
    Helper.logger("\n%s\t\t%s".format("OPERAND","N"))
    listener.operands.foreach { e =>
      val (token, freq) = e
      Helper.logger("%s\t\t%d".format(token,freq))
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
    val D = n1 * N2 / (2.toDouble * (if(n2 != 0) n2 else 1))
    
    // Compute effort
    val E = D * V
    
    // Compute LOC
    val lines = Source.fromFile(path).getLines.toList
    val loc = lines.length
    
    // Number blank lines
    val nbl = lines.filter { line => line.trim.length == 0 }.size
    
    val numChars = lines.foldLeft(0) { (sum,line) => sum + line.length() }
    
    val numUniqueTokens = listener.tokens.keySet.size

    val entropyTokens = entropy(listener.tokens)
    val entropyChars = entropy(listener.chars)
    
    val z = 8.87 - 0.033*V + 0.40*loc - 1.5*entropyTokens
    val logit = 1.0 / (1 + Math.pow(Math.E,-z))

    val ncl = numCommentLines(path,tokens)
    
//    Helper.logger("OPD: "+listener.operands)
//    Helper.logger("OPS: "+listener.operators)
//    Helper.logger("sentences: "+listener.sentences)
    
    val asl = analyzeAsl(tokens,listener.sentences)
    
    val awl = listener.lenWords / listener.numWords.toDouble
    
    val sres = asl - 0.1 * awl
    
    val cc = if(listener.decisions.size != 0) listener.decisions.values.sum + 1 else 1
    
    val ccnos = cc / Math.max(listener.numStatements,1).toDouble
    
    val cd = ncl / Math.max(listener.numStatements,1).toDouble
    
    val cm = ncl / loc
    
    val mi = 171 - 5.2 * Math.log(V) - 0.23 * cc * Math.log(loc)
    
    val misei = 171 - 5.2 * log2(V) - 0.23 * cc - 16.2 * log2(loc) + 50 * Math.sin(Math.sqrt(2.4*loc))
    
    val mims = Math.max(0, (171 - 5.2 * Math.log(V) - 0.23 * cc - 16.2 * Math.log(loc)) * 100.0 / 171.0)
    
    val readability = raykernel.apps.readability.eval.Main.getReadability(Source.fromFile(path).mkString)
    
    print("%-10s %6d %6d %6d %6d %6.2f %6.2f %7.1f %6.2f %6d %6.2f %6.2f %7.5f ".
        format(Helper.basename(path),loc,numChars,N,n,V,D,E,entropyTokens,numUniqueTokens,entropyChars,z,logit))    

    println("%6.2f %6d %6d %6d %7.2f %7.2f %7.2f %6.3f %6.3f %6.4f".
        format(sres, ncl, nbl, cc, mi, misei, mims, ccnos, cd, readability))
  }

  /**
   * Get the ASL.
   * NOTE: there's some ambiguity abut the relationship between ASL and AWL, in other words, is
   * ASL just the simple length of a sentence (what we calculate) or is it the sum of the adjusted
   * word lengths in a sentence.
   */
  def analyzeAsl(tokens: CommonTokenStream, sentences: HashMap[Int,Sentence]): Double = {       
    // Ignore whitespace: Abbas(2010), p25, paragraph 1, next to last sentence
    val total = sentences.foldLeft(0) { (sum, pair) =>
      val (num, sentence) = pair
      
      val startText = sentence.start.getText
      val stopText = sentence.stop.getText
      
      // Count lengths of tokens between start and stop tokens of this sentence
      val start = sentence.start.getTokenIndex
      val stop = sentence.stop.getTokenIndex
      
      val sentenceLen = (start to stop).foldLeft(0) { (len, index) =>          
        val token = tokens.get(index)
        
        val tokenText = token.getText
        
        // Get length of this "patch" of the sentence on the default (or non-hidden) channel        
        if(token.getChannel == Token.DEFAULT_CHANNEL) {

          val patchLen = token.getStopIndex - token.getStartIndex + 1
          
          len + patchLen
        }
        
        else
          len
      }
      
      // Curly to curly is sentence length, otherwise, subtract one for
      // "}" -> ";" or ";" -> ";" or "{" -> ";"
      if(startText == "{" && stopText == "}")
        sum + sentenceLen
      else
        sum + sentenceLen - 1
    }
    
    val asl = total / sentences.size.toDouble
    
    asl
  }
  
  /** Count number of commented lines ASSUMING!!! C.g4 puts block & single-line comments in HIDDEN channel */
  def numCommentLines(path: String, tokens:CommonTokenStream): Int = {
    val s = Source.fromFile(path).mkString
    
    val ncl = (0 until tokens.size).foldLeft(0) { (sum, index) =>
      // Look for tokens in the HIDDEN channel
      val token = tokens.get(index)
      
      if(token.getChannel != Token.DEFAULT_CHANNEL) {
        // If HIDDEN channel contains a newline, there is one additional line
        (token.getStartIndex to token.getStopIndex).foldLeft(1) { (commentSum, k) =>
          if(s(k) == '\n')
            commentSum + 1
          else
            commentSum
        } + sum
      }
      else
        sum
    } 
    
    ncl
  }
  
  def log2(value: Double): Double = Math.log(value) / LOG2
  
  def entropy(map: HashMap[String,Int]): Double = {
    val total = map.values.sum
    val h = map.foldLeft(0.0) { (sum,itemCount) =>
      val (item,count) = itemCount
      
      val p = count / total.toDouble
      sum - p * log2(p)
    }   
    
    h
  }
}

class HalsteadParserListener(tokenStream: CommonTokenStream) extends ParseTreeListener {
  val operators = new HashMap[String,Int]()
  val operands = new HashMap[String,Int]()
  val tokens = new HashMap[String,Int]()
  val chars = new HashMap[String,Int]()
  val decisions = new HashMap[String,Int]()
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
    arg match {
      case node: CParser.UnaryOperatorContext =>
        isUnaryOpContext = true
        Helper.logger("GOT UNARY!")
        
      case node: CParser.PrimaryExpressionContext =>
        isExprContext = true
        Helper.logger("GOT EXPR!")
        
      case node: CParser.InitDeclaratorContext =>
        isDeclarContext = true
        Helper.logger("GOT INIT DECLAR!")
        
      case node: CParser.CompoundStatementContext =>
        compoundSttDepth += 1
        Helper.logger("GOT COMPOUND!")
        
      case node: CParser.PostfixExpressionContext =>
        isPostfixContext = true
        Helper.logger("GOT POSTFIX!")
        
      case node: CParser.FunctionSpecifierContext =>
        Helper.logger("GOT FUNCTION!")
        
      case node: CParser.IterationStatementContext =>
        isIterationContext = true
        
      case _ =>
    }
  }
  
  def exitEveryRule(arg: ParserRuleContext): Unit = {
    arg match {
      case node: CParser.IterationStatementContext =>
        isIterationContext = false
        
      case _ =>
    }
  }
  
  def visitErrorNode(arg: ErrorNode): Unit = {}
  
  def visitTerminal(arg: TerminalNode): Unit = {
    halsteadProcessing(arg)
    
    sresProcessing(arg)
    
    ccProcessing(arg)
  }
  
  def ccProcessing(arg: TerminalNode): Unit = {
    val token = arg.getText
    token match {
      case "if" | "else" | "for" | "while" | "case" | "||" | "&&" | "?" =>
        val count = decisions.getOrElse(token, 0)
        decisions(token) = count + 1
      case _ =>
    }    
  }
  
  def halsteadProcessing(arg: TerminalNode): Unit = {  
    // TODO: test nested expressions
    // TODO: test multiple unary expressions, eg, i++ + -j + *--p
    val token = arg.getText
    
    val numTokens = tokens.getOrElse(token, 0)
    tokens(token) = numTokens + 1
    
    val numChars = chars
    token.foreach { c =>
      val s = c.toString
      val numChars = chars.getOrElse(s, 0)
      chars(s) = numChars + 1
    }
    
    val parent = arg.getParent
    val numChildren = parent.getChildCount
    val parentText = parent.getText

    // Do the Halstead calculation
    // See exmple in https://en.wikipedia.org/wiki/Halstead_complexity_measures
    token match {
      case token if isNumber(token) || isString(token) =>
        val count = operands.getOrElse(token,0)
        operands(token) = count + 1
      
      case token if isIdentifier(token) && isExprContext =>
        // If next token is a "(", count this as an operator
        val tokenIndex = arg.getSymbol.getTokenIndex
        if((tokenIndex+1) < tokenStream.size && tokenStream.get(tokenIndex+1).getText == "(") {  
          val count = operators.getOrElse(token,0)
          operators(token) = count + 1
        }
        else {
          val count = operands.getOrElse(token,0)
          operands(token) = count + 1

        }
        isExprContext = false
        
      case token if(isIdentifier(token) && isDeclarContext) =>
        val count = operands.getOrElse(token,0)
        operands(token) = count+ 1
        isDeclarContext = false
        
      case token if isFunction(token) =>
        // If next token is "(", count this as a function operator
        val tokenIndex = arg.getSymbol.getTokenIndex
        
        if((tokenIndex+1) < tokenStream.size && tokenStream.get(tokenIndex+1).getText == "(") {  
          val count = operators.getOrElse(token,0)
          operators(token) = count + 1
        }

      case "(" | "{" =>
          val rhs = if(token == "(") ")" else "}"
          val key = token + rhs
          val count = operators.getOrElse(key,0)
          operators(key) = count + 1
          
      case ("*" | "-" | "--" | "++" | "&" | "!" | "~") if isUnaryOpContext =>
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
           "!=" | "<=" | ">=" | "==" | "-=" | "+=" | "*=" | "/=" | "%=" |
           "||" | "&&" | "->" | "<<" | ">>" |
           "[" | 
           "?" =>

        if(isDeclarContext) {
          val key = "INIT"
          val states = parent.toStringTree()
          val op = parent.getChild(numChildren-1)
          val operatorCount = operators.getOrElse(key,0)
          operators(key) = operatorCount + 1 
          isDeclarContext = false
        }
        else {         
          val op = parent.getChild(numChildren-1).getText.trim
          val operatorCount = operators.getOrElse(op,0)
          operators(op) = operatorCount + 1
        }
        
      case _ =>
        // Put in key words as operators
        if(keyWords.contains(token)) {
          val operatorCount = operators.getOrElse(token,0)
          
          operators(token) = operatorCount + 1 
        }
    }

  }
  
  var numStatements = 0
  var numSentences = 0
  var numWords = 0
  var lenWords = 0
  var sawDot = false
  var isIterationContext = false
  var isForContext = false
  var semiCount = 0
  var lastStopIndex = 0
  val sentences = HashMap[Int, Sentence]()
  val stack = ListBuffer[Token]()
  var mark: Option[Token] = None
  
  def sresProcessing(arg: TerminalNode): Unit = {
    // TODO: test for statements
    val token = arg.getText
    Helper.logger("token: "+token)
    
    val len = token.length
        
    // Collection SRES statistics
    // NOTE: Abbas (2009), p26, item 7, nesting level appears not to be implemented
    // in Pojge.jar.
    
    token match {
      case "*" if isUnaryOpContext =>
        // TODO: seems like more processing needed here since it is like DOT
        numWords += 1
        
        lenWords += len
        
        sawDot = false
        
      case "for" =>
        numWords += 1
        
        lenWords += len
        
        isForContext = true
        
        sawDot = false

      case ";" =>
        handleSemicolons(arg.getSymbol)
        
        sawDot = false
          
      case "{" =>
        val startIndex = arg.getSymbol.getStartIndex
        val stopIndex = arg.getSymbol.getStopIndex
        
        push(arg.getSymbol)

        sawDot = false
      
      case "}" =>
        val startIndex = arg.getSymbol.getStartIndex
        val stopIndex = arg.getSymbol.getStopIndex 
        
        val lastToken = pop
        val lastStartIndex = lastToken.getStartIndex
        val lastStopIndex = lastToken.getStopIndex
        
        sentences(numSentences) = Sentence(lastToken,arg.getSymbol)
        
        // Go back this far with a ";" should it appear
        mark = Some(arg.getSymbol)
        
        numSentences += 1
        
      case tok: String if isIdentifier(tok) || isString(tok) || isNumber(tok) || keyWords.contains(tok) =>
        val trueCount = if(!sawDot) 1 else 2
        numWords += trueCount
      
        val trueLen = if(!sawDot) len else len * 2
        lenWords += trueLen
      
        sawDot = false
        
      case "." | "->" =>
        numWords += 2
        
        val trueLen = len * 2
        lenWords += trueLen
        
        sawDot = true
        
      case "-" | "--" | "++" | "&" | "!" | "~" |
           "++" | "--" |
           "=" | "+" | "/" | "<" | ">" | "." | "%" | "&" | "|" | "!" | "^" | "~" |
           "!=" | "<=" | ">=" | "==" | "-=" | "+=" | "*=" | "/=" | "%=" |
           "||" | "&&" | "->" | "<<" | ">>" |
           "[" | 
           "?" =>
             numWords += 1
             
             lenWords += len
             
             sawDot = false
      case _ =>
    }
  }

  /** Handle semicolons inside and outside FOR loop */
  def handleSemicolons(curToken: Token): Unit = {
    numStatements += 1
    
    // If we're in FOR loop, don't count semicolons as separate sentence, see Abbas (2010), p25, #4
    if (isForContext) {
      semiCount += 1

      if (semiCount == 2) {
        isForContext = false
        
        semiCount = 0
      }
      
      return
    }

    // Go back to last mark to measure sentence length
    mark match {
      case Some(lastToken) =>
        sentences(numSentences) = Sentence(lastToken, curToken)
        
      case None =>
        if (stack.size != 0)
          sentences(numSentences) = Sentence(stack.last, curToken)
        else
          // Case of code outside function block
          sentences(numSentences) = Sentence(curToken, curToken)
    }
    
    mark = Some(curToken)

    numSentences += 1
  }
  
  def push(token: Token): Unit = stack.append(token)
  def pop(): Token = stack.remove(stack.size - 1)
  
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
  
  def isFunction(s: String): Boolean = isIdentifier(s)
}