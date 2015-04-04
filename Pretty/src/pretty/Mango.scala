package pretty

import java.util.regex.Pattern

object Symbols {
  val keywords = List(
      "auto",
      "break",
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
  
  val delims = "[ \\[*-+\\|/\\?\\.><=%\\$#!\\^&)(,;]"

  def main(args:Array[String]): Unit = {
    val s = "static char *make_link_name (char const *name, char const *linkname);"
    
    val words = s.split(delims)
    words.foreach { word =>
      if(word.length > 0) {
        keywords.find(p => word == p) match {
          case Some(s) =>
          case None =>
            println(word)
        }
      }
    }
    
    val line1 = "/* ABC */ 123 /* def";
    val line2 = "xyz*/ hello world"
    val w = decommentBlock(List(line1,line2)).foreach(p => println(p))
    
//    val line3 = "hello // world"
//    decommentLine(List(line3)).foreach(p => println(p))
    
  }

  /** Removes line comments */
  def decommentLine(lines: List[String]): List[String] = {
    lines.foldLeft(List[String]()) { (composite,line) =>
      val quoteOpenIndex = line.indexOf("\"")
      val quoteCloseIndex = if(quoteOpenIndex == -1) -1 else line.substring(quoteOpenIndex+1).indexOf("\"")

      val startCommentIndex = line.indexOf("//")
      if(startCommentIndex == -1 || (startCommentIndex > quoteOpenIndex && startCommentIndex < quoteCloseIndex))
        composite ++ List(line)
      else
        composite ++ List(line.substring(0,startCommentIndex))
    }
  }
  
  /** Removes block comments */
  def decommentBlock(lines: List[String]): List[String] = {
    println("INPUT")
    lines.foreach(p => println(p))
    println("---")
    val OPENING_BLOCK = "/\\*"
    val CLOSING_BLOCK = "\\*/"
    val s = lines.foldLeft((false, List[String]())) { (inCommentLines, line) =>
      val inComment = inCommentLines._1
      val lines = inCommentLines._2

      val openings = line.split(OPENING_BLOCK)
//      println(openings.length)
//      (0 until openings.length).foreach { n =>
//        println(n + " : " + openings(n))
//      }
//      println("---")
      val startComment = line.contains("/*")
      val u = openings.foldLeft((startComment, "")) { (inCommentLine, words) =>
        val closings = words.split(CLOSING_BLOCK)
        val endComment = words.contains("*/")
        println("words:"+words+"||END COMMENT:"+endComment)
        val v = (0 until closings.length).foldLeft(inCommentLine._2) { (comments, n) =>
          if (n == 1)
            comments + closings(n)
          else
            comments
        }
        println("in comment:"+(!endComment)+" line:"+v)
        (!endComment||startComment, v)
      }
      (u._1,lines ++ List(u._2))
    }
    if(s._1 != false)
      println("WARNING detected possible unclosed block comment.")
    s._2
  }
}