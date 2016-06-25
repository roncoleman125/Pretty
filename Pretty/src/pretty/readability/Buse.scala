package pretty.readability

import io.Source
import pretty.util.Helper

object Buse {
  def main(args: Array[String]): Unit = {
    val path = args(0)
    val lines = Source.fromFile(path).mkString
    val readability = raykernel.apps.readability.eval.Main.getReadability(lines)
    
    val numLines = lines.count { c => c == '\n'}
    
    println("%10s %d %f".format(Helper.basename(path),numLines,readability))
  }
}