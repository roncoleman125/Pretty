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
package pretty

import java.util.regex.Pattern
import scala.collection.mutable.HashMap
import scala.io.Source
import pretty.util.Constants
import java.math.BigInteger
import java.util.prefs.Base64
import pretty.util.Config

object Mode extends Enumeration {
  val BEAUTIFY, DEBEAUTIFY = Value
}

object Mango {
//  val keywords = List(
//      "auto",
//      "break",
//      "bool",
//      "case",
//      "char",
//      "const",
//      "continue",
//      "default",
//      "do",
//      "double",
//      "else",
//      "enum",
//      "extern",
//      "float",
//      "for",
//      "goto",
//      "if",
//      "int",
//      "long",
//      "register",
//      "return",
//      "short",
//      "signed",
//      "sizeof",
//      "static",
//      "struct",
//      "switch",
//      "typedef",
//      "union",
//      "unsigned",
//      "void",
//      "volatile",
//      "while",
//      "true",
//      "false",
//      // Compiler directives
//      "define",
//      "include",
//      "ifdef",
//      "ifndef",
//      "endif",
//      "elif",
//      "undef",
//      "warning",
//      "line",
//      "pragma",
//      // C-library
//      "NULL",
//      "size_t",
//      "printf",
//      "sprintf",
//      "fprintf",
//      "open",
//      "close",
//      "fopen",
//      "fclose",
//      "fputs",
//      "fgets",
//      "stdout",
//      "stdin",
//      "stderr",
//      "free",
//      "malloc",
//      "main",
//      // Miscellaneous
//      "argc",
//      "argv"
//  )
  
//  val delims = "[ \\\\}\\{\\]\\[*\\-+\\|/\\?\\.><=%\\$#!\\^&)(,:;]"
  
  // Language keywords to skip
  val keywords = Config.getInstance().keywords

  // Language delimiters
  val delims = Config.getInstance().delims
  
  def main(args:Array[String]): Unit = { 
    val mango = new Mango(args(0))
    val lines = mango.filterStrings( mango.filterTabs(Source.fromFile("data/cat.c").getLines().toList))
       
    // Get only symbols of a given length
    val nocomments =  mango.filterComments(lines).
      filter(p => p.trim().length >= Constants.MIN_SYMBOL_SIZE)
    
    // Get only symbols of a given frequency or higher
    val symtab =  mango.buildSymbolTable(nocomments).
      filter(t => t._1.trim().length >= Constants.MIN_SYMBOL_SIZE).
        sortWith(_._1.length > _._1.length)  
        
    val trans =  mango.underbarSymbolize(symtab)
  }
}

/** This class implement the "mangling" code */
class Mango(path: String) {
  // Transform method of symbol to mango symbol
  val TRANSFORM: List[(String,Int)] => HashMap[String,String] = underbarSymbolize
  
  /** Returns the mappings of symbol to mango symbol */
  def getMappings(path: String): HashMap[String,String] = {
    if(Constants.verbose) println("creating mangos from "+path)
    
    // Remove tabs and strings
    if(Constants.verbose) println("removing strings and tabs...")
    val lines = filterStrings(filterTabs(Source.fromFile(path).getLines().toList))
//    if(Constants.verbose) lines.foreach(p => println(p))
       
    // Get only symbols of a given length or longer
    if(Constants.verbose) println("removing comments...")
    val nocomments = filterComments(lines)
    
    // Get only symbols of a given frequency or higher.
    // Note: sort by length from longer to shorter is important on chance
    // that some symbols may be contained in others.
    if(Constants.verbose) println("building symbol table...")
    val symtab = buildSymbolTable(nocomments).
      filter(t => t._1.trim().length >= Constants.MIN_SYMBOL_SIZE).filter(t => t._2 >= Constants.MIN_SYMBOL_FREQUENCY).
        sortWith(_._1.length > _._1.length)  
        
    if(Constants.verbose) println("building transformation table...")
    val trans = TRANSFORM(symtab)
    
//    if(Constants.verbose) trans.foreach { t => println(t._1+" => "+t._2)}
    
    trans
  }
  
  /** Get the mappings: old symbols -> new symbol */
  def getMappings(path: String, mode: Mode.Value, symLenFilter: ((String,Int)) => Boolean, symFreqFilter: ((String,Int)) => Boolean ): HashMap[String,String] = {
    if(Constants.verbose) println("creating mangos from "+path)
    
    // Remove tabs and strings
    if(Constants.verbose) println("removing strings and tabs...")
    val lines = filterStrings(filterTabs(Source.fromFile(path).getLines().toList))
//    if(Constants.verbose) lines.foreach(p => println(p))
       
    // Get only symbols of a given length or longer
    if(Constants.verbose) println("removing comments...")
    val nocomments = filterComments(lines)
    
    // Get only symbols of a given frequency or higher.
    // Note: sort by length from longer to shorter is important on chance
    // that some symbols may be contained in others.
    if(Constants.verbose) println("building symbol table...")
    val symtabUnfiltered = buildSymbolTable(nocomments)
    val symtab = symtabUnfiltered.filter(symFreqFilter).filter(symLenFilter).sortWith(_._1.length > _._1.length).
      filter(t => isIdentifier(t._1))
    if(Constants.verbose) symtab.foreach { t => println(t._1 + " => "+t._2)}
        
    if(Constants.verbose) println("building transformation table...")
    val trans = mode match {
      case Mode.BEAUTIFY =>
        doubleLengthSymbolize(symtab)
        
      case Mode.DEBEAUTIFY =>
        underbarSymbolize(symtab)
    }
    
//    if(Constants.verbose) trans.foreach { t => println(t._1+" => "+t._2)}
    
    trans
  }
  
  /** Returns true if the string is an identifier */
  def isIdentifier(s: String): Boolean = {
    if(s.length == 0 || !(s(0).isLetter || s(0) == '_' ))
        return false
    val t = s.substring(1)
    
    for(i <- 0 until t.length)
      if(!(t(i).isLetterOrDigit || t(i) == '_'))
        return false
    true
  }
  
  
  /** Returns the mapping from old symbol -> new symbol double length */
  def doubleLengthSymbolize(symtab: List[(String,Int)]): HashMap[String,String] = {
    (0 until symtab.length).foldLeft(HashMap[String,String]()) { (newTable, k) =>
      val oldSym = symtab(k)._1
      val newSym = oldSym.length match {
        case 1 =>
          oldSym + oldSym + oldSym + oldSym
        case 2 =>
          oldSym + oldSym
        case 3 =>
          oldSym + "_"
        case _ =>
          oldSym + oldSym
      }
      newTable(oldSym) = newSym
      newTable
    }
  }
  
  /** Returns the mapping from old symbol -> new symbol with underbar */
  def underbarSymbolize(symtab: List[(String,Int)]): HashMap[String,String] = {
    if (Constants.verbose) {
      val easies = symtab.foldLeft(0) { (count, sym) =>
        if (sym._1.contains("_"))
          count + sym._2
        else
          count
      }
      println("transformations to make: " + easies)
    }
    
    symtab.foldLeft(HashMap[String,String]()) { (trans,sym) =>
      val oldSym = sym._1
      
      // Use only symbols with underbar
      if(oldSym.contains("_")) {
        // New symbol will be first word plus _ plus first letter of each subsequent word
        val words = oldSym.split("_")
        val newSym = (1 until words.length).foldLeft(words(0)+"_") { (composite,k) =>
          val word = words(k)
          if(word.length > 0) composite + word(0) else composite
        }
        // If old and new syms are same, fall back to using first letter of each fragments
        if(newSym == oldSym) {
          val newSym2 = words.foldLeft("") { (composite,word) => composite+word(0) }
          if(trans.contains(newSym2)) Console.err.println(oldSym+" => "+newSym2+" collision")
          trans(oldSym) = newSym2
        }
        else {
          if(trans.contains(newSym)) Console.err.println(oldSym+" => "+newSym+" collision")
          trans(oldSym) = newSym
        }
      }
      else if (oldSym.filter(p => !p.isDigit).length == 0) {
        // Skip numbers
      }
      else if(oldSym.toUpperCase == oldSym) {
        // If symbol all upper case, use every other character
        val newSym = (0 until oldSym.length).foldLeft("") { (composite,k) =>
          if(k % 2 == 0) composite + oldSym(k) else composite
        }
        if(trans.contains(newSym)) Console.err.println(oldSym+" => "+newSym+" collision")
        trans(oldSym) = newSym
      }
      else if(oldSym.length >= Constants.MIN_SYMBOL_SIZE) {
        // If symbol has no clues, reduce symbol by half
        val newSym = (0 until oldSym.length/2).foldLeft("") { (composite, k) =>
          composite + oldSym(k)
        }
//        println(oldSym + " => " + newSym)
        if(trans.contains(newSym)) Console.err.println("symbol collision: "+oldSym+" => "+newSym)
        trans(oldSym) = newSym
      }
      else
        Console.err.println("bad mapping for "+oldSym)

      trans
    }
  }
  
  /** Returns the mapping from old symbol -> new symbol with count as part of name */
  def countSymbolize(symtab: List[(String,Int)]): HashMap[String,String] = {
    (0 until symtab.length).foldLeft(HashMap[String,String]()) { (newTable, k) =>
      val oldSym = symtab(k)._1
      val newSym = "v" + k
      newTable(oldSym) = newSym
      newTable
      }
  }
  
  /** Returns the mapping from old symbol -> new symbol reduced by half */
  def halfSymbolize(symtab: List[(String,Int)]): HashMap[String,String] = {
    symtab.foldLeft(HashMap[String,String]()) { (trans,sym) =>
      val oldSym = sym._1
      val newSym = (0 to oldSym.length/2).foldLeft("") { (composite,k) =>
        composite + oldSym(k)
      }
      trans(oldSym) = newSym
      trans
    }
  }
  
  /** Returns the symbol table as a list of 2-tuples, (symbol,frequency). */
  def buildSymbolTable(lines: List[String]): List[(String,Int)] = {
    val symbolTable = new HashMap[String,Int]().withDefaultValue(0)
    
    lines.foreach { line =>
      val words = line.split(Mango.delims+"+")
      words.foreach { word =>
        val cleanWord = word.trim.replace("\"","")
        if (word.length > 0) {
          Mango.keywords.find(p => cleanWord == p) match {
            case Some(s) =>
            case None =>
              symbolTable(cleanWord) += 1
          }
        }
      }
    }
    symbolTable.toList
  }
  
  /** Returns path without comments */
  def decomment: List[String] = {
    filterComments(filterTabs(Source.fromFile(path).getLines().toList))
  }
  
  /** Returns tabs replaced with spaces. */
  def filterTabs(lines: List[String]): List[String] = {
    lines.map { line =>
      val buffer = new StringBuffer("")
      
      line.foldLeft((1,"")) { (posNewLine,c) =>
        val pos = posNewLine._1
        val newLine = posNewLine._2
        
        val offset = c match {
          case '\t' =>
            // Pad is distance in spaces to next tab stop
            val padAmt = Constants.TAB_STOP - (pos % Constants.TAB_STOP)
            for(i <- 0 until padAmt) buffer.append(' ')
            padAmt
          case _ =>
            buffer.append(c)
            1
        }
        (pos+offset,buffer.toString())
        } . _2
    }
  }

  //  /** Returns lines without line-oriented comments. */
  //  def filterLineComments(lines: List[String]): List[String] = {
  //    lines.foldLeft(List[String]()) { (composite,line) =>
  //      val startCommentIndex = line.indexOf("//")
  //      if(startCommentIndex == -1)
  //        composite ++ List(line)
  //      else
  //        composite ++ List(line.substring(0,startCommentIndex-1))
  //    }
  //  }

  /** Returns lines without strings. */
  def filterStrings(lines: List[String]): List[String] = {
    // code "..." code
    val SCENARIO_A = """(.*)\".*\"(.*)""".r

    // code "...
    val SCENARIO_B = """(.*)\".*""".r

    // ..." code
    val SCENARIO_C = """.*\"(.*)""".r

    val output = (0 until lines.length).foldLeft((false, List[String]())) { (stringLines, k) =>

      val inComment = stringLines._1

      // Get rid of trim spaces and escaped quotes
      val line = lines(k). /*trim .*/ replaceAll("\\\\\"", "@")

      line match {
        case SCENARIO_A(codeBefore, codeAfter) =>
          (false, stringLines._2 ++ List(codeBefore + "\"" + "\"" + codeAfter))

        case SCENARIO_B(code) if !inComment =>
          (true, stringLines._2 ++ List(code + "\""))

        case SCENARIO_C(code) if inComment =>
          (false, stringLines._2 ++ List("\"" + code))

        case _ =>
          if (inComment)
            (true, stringLines._2)
          else
            (false, stringLines._2 ++ List(line))
      }
    }

    output._2
  }
  
  /** Filters comments from a list of strings */
  def filterComments(lines: List[String]): List[String] = {   
    // /* block comment */ code
    val SCENARIO_A = """/\*.*\*/(.*)""".r
    
    // code /* block comment start
    val SCENARIO_B = """(.*)/\*.*""".r
    
    // block comment end */ code
    val SCENARIO_C = """.*\*/(.*)""".r
    
    // code /* block comment */
    val SCENARIO_D = """(.*)/\*.*\*/""".r
    
    // code /* block comment */ code
    val SCENARIO_E = """(.*)/\*.*\*/(.*)""".r
    
    // code // single line comment
    val SCENARIO_F = """(.*)//.*""".r
    
    val output = (0 until lines.length).foldLeft((false,List[String]())) { (commentLines,k) =>
      val inComment = commentLines._1
      val line = lines(k) //.trim
//      if(k >= 0) {
//        println(line)
//        print("")
//      }

      line.trim match {
        case SCENARIO_A(code) =>
          checkState(inComment,false,k+1)
            
          if(code.trim.length != 0) (false,commentLines._2 ++ List(code)) else (false,commentLines._2)

        case SCENARIO_D(code) =>
          checkState(inComment,false,k+1)
          
          if(code.trim.length != 0) (false,commentLines._2 ++ List(code)) else (false,commentLines._2)
          
        case SCENARIO_E(codeBefore,codeAfter) =>
          checkState(inComment,false,k+1)
          
          if(codeBefore.trim.length != 0 && codeAfter.trim.length != 0)
            (false,commentLines._2 ++ List(codeBefore,codeAfter))
          else if(codeBefore.trim.length != 0)
              (false,commentLines._2 ++ List(codeBefore))
          else
            (false,commentLines._2 ++ List(codeAfter))
          
        case SCENARIO_B(code) =>
          checkState(inComment,false,k+1)
          
          if(code.trim.length != 0) (true,commentLines._2 ++ List(code)) else (true,commentLines._2)
          
        case SCENARIO_C(code) =>
          checkState(inComment,true,k+1)
            
          if(code.trim.length != 0) (false,commentLines._2 ++ List(code)) else (false,commentLines._2)

        case SCENARIO_F(code) =>
           if(code.trim.length != 0) (inComment,List(code) ++ commentLines._2) else (inComment,commentLines._2)
          
        case _ =>
          if(inComment)
            (inComment,commentLines._2)
          else
            (inComment, commentLines._2 ++ List(line))
      }
    }
    
    if(output._1 != false)
      Console.err.println("warning reached end of lines with open comment block.")
      
    output._2
  }
  
  /** Checks state of processing */
  def checkState(actual: Boolean, expected: Boolean,lineno: Int) {
    if(actual != expected)
      Console.err.println("warning detected possible bad block comment: line "+lineno)
  }
  
  /** Logs the symbol table verbose */
  def report(symtab: List[(String,Int)]) {
    val easies = symtab.foldLeft(0) { (count, sym) =>
      if(sym._1.contains("_"))
        count+sym._2
      else
        count
      }
    
    val hards = symtab.foldLeft(0) { (count, sym) =>
      if(!sym._1.contains("_"))
        count+sym._2
      else
        count
      }
    println("viable symbols to mango: "+symtab.length)
    println("potential easies: "+easies)
    println("potential hards: "+hards)
    symtab.foreach { p =>
      println("%3d %s".format(p._2,p._1))
    }
  }
  
  /** Logs the symbols table */
  def report(trans: HashMap[String,String]) {
    println("trans table:")
    trans.foreach { p =>
      println(p._1+" => "+p._2)
    }    
  }
}