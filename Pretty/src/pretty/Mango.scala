package pretty

import java.util.regex.Pattern
import scala.collection.mutable.HashMap
import scala.io.Source

object Mango {
  val keywords = List(
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
  
  val TAB_STOP = 4
  val MIN_SYMBOL_SIZE = 3
  val MIN_SYMBOL_FREQUENCY = 3

  def main(args:Array[String]): Unit = {  
//    val t = s"#define MIN_COLUMN_WIDTH\t3"
//    val t = s"ABCD\t\t3"
//
//    println(t.length)
//    
//    val ls = filterTabs(List(t))
//    println("12345678901234567890")
//    println(ls(0))
    
    val lines = filterStrings(filterTabs(Source.fromFile("data/cat.c").getLines().toList))
       
    // Get only symbols of a given length
    val nocomments = filterComments(lines).
      filter(p => p.trim().length >= MIN_SYMBOL_SIZE)
    
    // Get only symbols of a given frequency or higher
    val symtab = buildSymbolTable(nocomments).
      filter(t => t._1.trim().length >= MIN_SYMBOL_FREQUENCY).
        sortWith(_._1.length > _._1.length)
    
    println("viable symbols to mango: "+symtab.length)
    symtab.foreach { p =>
      println("%3d %s".format(p._2,p._1))
    }   
  }
  
  /** Returns the symbol table as a list of 2-tuples, (symbol,frequency). */
  def buildSymbolTable(lines: List[String]): List[(String,Int)] = {
    val symbolTable = new HashMap[String,Int]().withDefaultValue(0)
    
    lines.foreach { line =>
      val words = line.split(delims+"+")
      words.foreach { word =>
        val cleanWord = word.trim.replace("\"","")
        if (word.length > 0) {
          keywords.find(p => cleanWord == p) match {
            case Some(s) =>
            case None =>
              symbolTable(cleanWord) += 1
          }
        }
      }
    }
    symbolTable.toList
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
            val padAmt = TAB_STOP - (pos % TAB_STOP)
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
  
  /** Returns lines without line-oriented comments. */
  def filterLineComments(lines: List[String]): List[String] = {
    lines.foldLeft(List[String]()) { (composite,line) =>
      val startCommentIndex = line.indexOf("//")
      if(startCommentIndex == -1)
        composite ++ List(line)
      else
        composite ++ List(line.substring(0,startCommentIndex-1))
    }
  }

  /** Returns lines without strings. */
  def filterStrings(lines: List[String]): List[String] = {
    // code "..." code
    val SCENARIO_A = """(.*)\".*\"(.*)""".r
    
    // code "...
    val SCENARIO_B = """(.*)\".*""".r
    
    // ..." code
    val SCENARIO_C = """.*\"(.*)""".r
    
    val output = (0 until lines.length).foldLeft((false,List[String]())) { (stringLines,k) =>
      if(k > 4568)
        k
        
      val inComment = stringLines._1
      
      // Get rid of trim spaces and escaped quotes
      val line = lines(k).trim .replaceAll("\\\\\"","@")
      
      line match {
        case SCENARIO_A(codeBefore,codeAfter) =>
          (false,stringLines._2 ++ List(codeBefore+"\""+"\""+codeAfter))
          
        case SCENARIO_B(code) if !inComment =>           
          (true,stringLines._2 ++ List(code+"\""))
          
        case SCENARIO_C(code) if inComment =>            
          (false,stringLines._2 ++ List("\""+code))
          
        case _ =>
          if(inComment)
            (true,stringLines._2)
          else
            (false,stringLines._2 ++ List(line))
      }
    }
    
    output._2
  }
  
  def filterComments(lines: List[String]): List[String] = {   
    // /* comment */ code
    val SCENARIO_A = """/\*.*\*/(.*)""".r
    
    // code /* comment
    val SCENARIO_B = """(.*)/\*.*""".r
    
    // */ code
    val SCENARIO_C = """.*\*/(.*)""".r
    
    // code /* comment */
    val SCENARIO_D = """(.*)/\*.*\*/""".r
    
    // code /* comment */ code
    val SCENARIO_E = """(.*)/\*.*\*/(.*)""".r
    
    // code //
    val SCENARIO_F = """(.*)//.*""".r
    
    val output = (0 until lines.length).foldLeft((false,List[String]())) { (commentLines,k) =>
      val inComment = commentLines._1
      val line = lines(k).trim
//      if(k >= 0) {
//        println(line)
//        print("")
//      }

      line match {
        case SCENARIO_A(code) =>
          (false,commentLines._2 ++ List(code))
          
        case SCENARIO_B(code) =>
          (true,commentLines._2 ++ List(code))
          
        case SCENARIO_D(code) =>
          (false,commentLines._2 ++ List(code))
          
        case SCENARIO_E(codeBefore,codeAfter) =>
          (false,commentLines._2 ++ List(codeBefore,codeAfter))
          
        case SCENARIO_C(code) =>
          if(!inComment)
            println("warning detected possible bad block comment line "+(k+1))
            
          (false,commentLines._2 ++ List(code))

        case SCENARIO_F(code) =>
          (inComment,List(code) ++ commentLines._2)
          
        case _ =>
          if(inComment)
            (inComment,commentLines._2)
          else
            (inComment, commentLines._2 ++ List(line))
      }
    }
    
    if(output._1 != false)
      println("warning reached end of lines with open comment block.")
      
    output._2
  }
}