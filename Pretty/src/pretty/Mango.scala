package pretty

import java.util.regex.Pattern
import scala.collection.mutable.HashMap
import scala.io.Source
import pretty.util.Constants
import java.math.BigInteger
import java.util.prefs.Base64

object Mango {
  val skipwords = List(
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
      "pragma",
      // C-library
      "NULL",
      "size_t",
      "printf",
      "sprintf",
      "fprintf",
      "open",
      "close",
      "fopen",
      "fclose",
      "fputs",
      "fgets",
      "stdout",
      "stdin",
      "stderr",
      "free",
      "malloc",
      "main",
      // Miscellaneous
      "argc",
      "argv"
  )
  
  val delims = "[ \\\\}\\{\\]\\[*\\-+\\|/\\?\\.><=%\\$#!\\^&)(,:;]"

  
  def main(args:Array[String]): Unit = { 
    val mango = new Mango(args(0))
    val lines = mango.filterStrings( mango.filterTabs(Source.fromFile("data/cat.c").getLines().toList))
       
    // Get only symbols of a given length
    val nocomments =  mango.filterComments(lines).
      filter(p => p.trim().length >= Constants.MIN_SYMBOL_SIZE)
    
    // Get only symbols of a given frequency or higher
    val symtab =  mango.buildSymbolTable(nocomments).
      filter(t => t._1.trim().length >= Constants.MIN_SYMBOL_FREQUENCY).
        sortWith(_._1.length > _._1.length)  
        
    val trans =  mango.underbarSymbolize(symtab)
  }
}

class Mango(path: String) {
  // Transform method of symbol to mango symbol
  val TRANSFORM: List[(String,Int)] => HashMap[String,String] = underbarSymbolize
  
  /** Returns the mappings of symbol to mango symbol */
  def getMappings(path: String): HashMap[String,String] = {
    if(Constants.verbose) println("creating mangos from "+path)
    
    // Remove tabs and strings
    if(Constants.verbose) println("removing strings and tabs...")
    val lines = filterStrings(filterTabs(Source.fromFile(path).getLines().toList))
     if(Constants.verbose) lines.foreach(p => println(p))
       
    // Get only symbols of a given length or longer
    if(Constants.verbose) println("removing comments...")
    val nocomments = filterComments(lines)
    
    // Get only symbols of a given frequency or higher.
    // Note: sort by length from longer to shorter is important on chance
    // that some symbols may be contained in others.
    if(Constants.verbose) println("building symbol table...")
    val symtab = buildSymbolTable(nocomments).
      filter(t => t._1.trim().length >= Constants.MIN_SYMBOL_FREQUENCY).filter(t => t._2 >= Constants.MIN_SYMBOL_FREQUENCY).
        sortWith(_._1.length > _._1.length)  
        
    if(Constants.verbose) println("building transformation table...")
    val trans = TRANSFORM(symtab)
    
//    if(Constants.verbose) trans.foreach { t => println(t._1+" => "+t._2)}
    
    trans
  }
  
  def getMappings(path: String, symLenFilter: ((String,Int)) => Boolean, symFreqFilter: ((String,Int)) => Boolean ): HashMap[String,String] = {
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
    symtab.foreach { t => println(t._1 + " => "+t._2)}
        
    if(Constants.verbose) println("building transformation table...")
    val trans = doubleLengthSymbolize(symtab)
    
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
  
    /** Returns the transformation table */
  def underbarSymbolize(symtab: List[(String,Int)]): HashMap[String,String] = {
    if(Constants.verbose) {
    val easies = symtab.foldLeft(0) { (count, sym) =>
      if(sym._1.contains("_"))
        count+sym._2
      else
        count
      }
      println("transformations to make: "+easies)
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
          trans(oldSym) = newSym2
        }
        else
          trans(oldSym) = newSym
      }
      else if (oldSym.filter(p => !p.isDigit).length == 0) {
        // Skip numbers
      }
      else if(oldSym.toUpperCase == oldSym) {
        // If symbol all upper case, use every other character
        val newSym = (0 until oldSym.length).foldLeft("") { (composite,k) =>
          if(k % 2 == 0) composite + oldSym(k) else composite
        }
        trans(oldSym) = newSym
      }
      else {
        // If symbol has no clues, split name in half
        val newSym = (0 to oldSym.length/2).foldLeft("") { (composite, k) =>
          composite + oldSym(k)
        }
        trans(oldSym) = newSym
      }

      trans
    }
  }
  
  /** Returns new symbols as symbol plus count */
  def countSymbolize(symtab: List[(String,Int)]): HashMap[String,String] = {
    (0 until symtab.length).foldLeft(HashMap[String,String]()) { (newTable, k) =>
      val oldSym = symtab(k)._1
      val newSym = "v" + k
      newTable(oldSym) = newSym
      newTable
      }
  }
  
  /** Returns new symbols as symbol cut in half */
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
          Mango.skipwords.find(p => cleanWord == p) match {
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
  
  def checkState(actual: Boolean, expected: Boolean,lineno: Int) {
    if(actual != expected)
      Console.err.println("warning detected possible bad block comment: line "+lineno)
  }
  
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
  
  def report(trans: HashMap[String,String]) {
    println("trans table:")
    trans.foreach { p =>
      println(p._1+" => "+p._2)
    }    
  }
}