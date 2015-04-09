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
      // Miscellaneous
      "NULL",
      "size_t",
      "printf"
  )
  
  val delims = "[ \\}\\{\\]\\[*-+\\|/\\?\\.><=%\\$#!\\^&)(,:;]"

  def main(args:Array[String]): Unit = {
    val s = "static char *make_link_name (char const *name, char const *linkname);"
    
//    val words = s.split(delims)
//    words.foreach { word =>
//      if(word.length > 0) {
//        keywords.find(p => word == p) match {
//          case Some(s) =>
//          case None =>
//            println(word)
//        }
//      }
//    }
    

//    val line3 = "xyz*/"
//    val words = line3.split("\\*/")
////    val w = filterBlockComments(List(line1,line2)).foreach(p => println(p))
//    
////    val line3 = "hello // world"
////    decommentLine(List(line3)).foreach(p => println(p))
//    val lines = Source.fromFile("data/ls.c").getLines().toList
////    val lines = List("#include <config.h>","#include <stdio.h>","#include <getopt.h>","#include <sys/types.h>")
//    
//    val nocomments = filterBlockComments(lines).filter(p => p.trim().length >= 3)
////    println(nocomments)
//    
//    val symtab = buildSymbolTable(nocomments).filter(t => t._1.trim().length >= 3)
//    println("viable symbols to mango: "+symtab.length)
//    println(symtab)
    
  }
  
 

  def buildSymbolTable(lines: List[String]): List[(String,Int)] = {
    val symbolTable = new HashMap[String,Int]().withDefaultValue(0)
    
    lines.foreach { line =>
      val words = line.split(delims+"+")
      words.foreach { word =>
        val cleanWord = clean(word)
        if (word.length > 0) {
          keywords.find(p => cleanWord == p) match {
            case Some(s) =>
            case None =>
              symbolTable(cleanWord) += 1
          }
        }
      }
    }
    symbolTable.toList.sortWith(_._2 > _._2)
  }
  
  def clean(s: String): String = {
    s.trim.replace("\"","")
  }

  /** Removes strings */

  /** Removes line comments */
  def filterLineComments(lines: List[String]): List[String] = {
    lines.foldLeft(List[String]()) { (composite,line) =>
      val startCommentIndex = line.indexOf("//")
      if(startCommentIndex == -1)
        composite ++ List(line)
      else
        composite ++ List(line.substring(0,startCommentIndex-1))
    }
  }

  
  def filterStrings(lines: List[String]): List[String] = {
    // code "..." code
    val SCENARIOA = """(.*)\".*\"(.*)""".r
    
    // code "...
    val SCENARIOB = """(.*)\".*""".r
    
    // ..." code
    val SCENARIOC = """.*\"(.*)""".r
    null
  }
//  val line0 = """GOOG:310.84"""
//    val line1 = "/* ABC */"
//    val line2 = "xyz*/ hello world"
//    filterBlockComments2(List(line1,line2)).foreach(p => println(p))  
  val lines = Source.fromFile("data/ls.c").getLines().toList
  filterBlockComments(lines).foreach(p => println(p))
  
  def filterBlockComments(lines: List[String]): List[String] = {   
    // /* comment */ code
    val SCENARIO0 = """/\*.*\*/(.*)""".r
    
    // code /* comment
    val SCENARIO1 = """(.*)/\*.*""".r
    
    // */ code
    val SCENARIO2 = """.*\*/(.*)""".r
    
    // code /* comment */
    val SCENARIO3 = """(.*)/\*.*\*/""".r
    
    // code /* comment */ code
    val SCENARIO4 = """(.*)/\*.*\*/(.*)""".r
    
    // code //
    val SCENARIO5 = """(.*)//.*""".r
    
    val output = (0 until lines.length).foldLeft((false,List[String]())) { (commentLines,k) =>
      val inComment = commentLines._1
      val line = lines(k).trim
//      if(k >= 0) {
//        println(line)
//        print("")
//      }

      line match {
        case SCENARIO0(code) =>
          (false,commentLines._2 ++ List(code))
          
        case SCENARIO1(code) =>
          (true,commentLines._2 ++ List(code))
          
        case SCENARIO3(code) =>
          (false,commentLines._2 ++ List(code))
          
        case SCENARIO4(codeBefore,codeAfter) =>
          (false,commentLines._2 ++ List(codeBefore,codeAfter))
          
        case SCENARIO2(code) =>
          if(!inComment)
            println("warning detected possible bad block comment line "+k)
            
          (false,commentLines._2 ++ List(code))

        case SCENARIO5(code) =>
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