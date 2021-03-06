package pretty.readability

import java.io._
import java.nio.charset.StandardCharsets

import scala.collection.JavaConverters._
import scala.collection.mutable.HashMap
import scala.math

import org.antlr.v4.runtime._

import cgrammar.CLexer
import cgrammar.CParser

object Parser {
  val Y_INTERCEPT       = 8.87
  val LINE_COUNT_COEF   = 0.4
  val BYTE_ENTROPY_COEF = -1.5
  val VOLUME_COEF       = -0.033

  val excludedTokens = Set("<EOF>")

}

class Parser(string: String) {
  private var characterMap = scala.collection.mutable.HashMap.empty[Char,Int]

  def generateAST() {
    var in = new ANTLRInputStream
        (new ByteArrayInputStream(string.getBytes(StandardCharsets.UTF_8)))
    var lexer = new CLexer(in)
    var tokens = new CommonTokenStream(lexer)
    var parser = new CParser(tokens)
    var rulesList = List(parser.getRuleNames().toSeq: _*)
    println( parser.functionDefinition().toStringTree(
      rulesList.asJava ) )
  }

  def getLineCount(): Int = {
    val count = string.lines.count((s: String) => true)
    count
  }

  private def percentOccurrence(c: Char): Double = {
    return characterMap(c).toDouble / string.length
  }
  
  private def logProduct(c: Char): Double = {
    var p = percentOccurrence(c)
    return p * (math.log(p) / math.log(2))
  }

  def getByteEntropy(): Double = {
    for ( c <- string.toCharArray ) {
      characterMap += (c -> (characterMap.getOrElse(c, 0) + 1))
    }

    var entropySum = 0.0
    for ( kv <- characterMap ) {
      entropySum += logProduct(kv._1)
    }
    return -entropySum
  }

  private def setErrorListener(lexer: org.antlr.v4.runtime.Lexer) {
    lexer.removeErrorListeners
    lexer.addErrorListener(new BaseErrorListener() {
      override def syntaxError(a: org.antlr.v4.runtime.Recognizer[_,_], b: Any, c: Int, d: Int, e: String, f: org.antlr.v4.runtime.RecognitionException): Unit = {
        throw new Throwable("Syntax error encoutered")
      }
    });
  }
  
  def getVolume(): Double = {
    var in = new ANTLRInputStream(
        new ByteArrayInputStream(string.getBytes(StandardCharsets.UTF_8)))
    var lexer = new CLexer(in)
    setErrorListener(lexer)
    var tokenStream = new CommonTokenStream(lexer)
    try {
      tokenStream.fill()
    } catch {
      // Return NaN if there was a syntax error
      case e: java.lang.Throwable => return Double.NaN
    }
    var rawTokens = tokenStream.getTokens()
    var tokens = scala.collection.mutable.MutableList.empty[String]
    for ( i <- 0 until rawTokens.size ) {
      var token = rawTokens.get(i).getText
      if ( !Parser.excludedTokens.contains(token) )
        tokens += token
    }
    var tokenSet = scala.collection.mutable.Set.empty[String]
    for ( token <- tokens ) {
      if ( !Parser.excludedTokens.contains(token) )
        tokenSet += token
    }
    
    val N = tokens.size
    val n = tokenSet.size
    
    return tokens.size * (math.log(tokenSet.size) / math.log(2))
  }

  def getRegressionVariable: Double = {
    return Parser.Y_INTERCEPT
        .+(Parser.LINE_COUNT_COEF * getLineCount)
        .+(Parser.BYTE_ENTROPY_COEF * getByteEntropy)
        .+(Parser.VOLUME_COEF * getVolume)
  }

  def getLogitValue: Double = {
    return 1 / (1 + math.pow(math.E, -getRegressionVariable))
  }
}
