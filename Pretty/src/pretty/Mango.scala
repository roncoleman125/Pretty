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
      "pragma"
  )
  
  val miscwords = List(
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
    
    val line1 = "/* ABC */ 123 /* def"
    val line2 = "xyz*/ hello world"
    val line3 = "xyz*/"
    val words = line3.split("\\*/")
//    val w = filterBlockComments(List(line1,line2)).foreach(p => println(p))
    
//    val line3 = "hello // world"
//    decommentLine(List(line3)).foreach(p => println(p))
    val lines = Source.fromFile("data/ls.c").getLines().toList
//    val lines = List("#include <config.h>","#include <stdio.h>","#include <getopt.h>","#include <sys/types.h>")
    
    val nocomments = filterBlockComments(lines).filter(p => p.trim().length >= 3)
//    println(nocomments)
    
    val symtab = buildSymbolTable(nocomments).filter(t => t._1.trim().length >= 3)
    println("viable symbols to mango: "+symtab.length)
    println(symtab)
    
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
  def filterStrings(lines: List[String]): List[String] = {
    lines.foldLeft(List[String]()) { (composite, line) =>
      if (line.count(p => p == '"') > 2)
        composite
      else {
        val startString = line.indexOf("\"")
        val endString = line.lastIndexOf("\"")
        if (startString == -1 || startString == endString)
          composite ++ List(line)
        else {
          composite ++ List(line.substring(0, startString) + line.substring(endString + 1))
        }
      }
    }
  }

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

  /** Removes block comments */
  def filterBlockComments(lines: List[String]): List[String] = {
    val OPENING_BLOCK = "/\\*"
    val CLOSING_BLOCK = "\\*/"
    val s = lines.foldLeft((false, List[String]())) { (inCommentLines, line) =>
      val inComment = inCommentLines._1
      val lines = inCommentLines._2
//      println(">>" + line)

      val openings = line.split(OPENING_BLOCK)

      val startComment = line.contains("/*")
      val endComment = line.contains("*/")
      val u = openings.foldLeft((startComment || inComment, "")) { (inCommentLine, words) =>
        val inComment = inCommentLine._1
        val line = inCommentLine._2
        val closings = (words + " ").split(CLOSING_BLOCK)
        if (closings.length == 1 && !inComment)
          (inComment, words)
        else {
          val v = (0 until closings.length).foldLeft(line) { (comments, n) =>
            if (n == 1)
              comments + closings(n)
            else
              comments
          }
//          println((!endComment) + " || line:" + v)
          (inComment && !endComment, v)
        }
      }
      (u._1, lines ++ List(u._2))
    }
    if (s._1 != false)
      println("WARNING detected possible unclosed block comment.")
    s._2
  }
}