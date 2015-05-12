package pretty

import scala.io.Source
import pretty.util.Imagery

object SingleImage {
  def main(args: Array[String]): Unit = {
    val lines = Source.fromFile(args(0)).getLines().toList
    
    val width = lines.foldLeft(0) { (width, line) =>
      val len = line.length
      if (len > width) len else width
    }
    
    Imagery.getInstance(args(0), "base", true)
  }

}