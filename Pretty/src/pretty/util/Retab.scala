package pretty.util

import scala.io.Source
import pretty.JavaHelper

object Retab {
  def main(args: Array[String]): Unit = {
    val lines = Source.fromFile(args(0)).getLines.toList
    val tabstop = args(1).toInt
    
    lines.foreach { line =>
      println(JavaHelper.retab(line, tabstop))
    }
  }
}