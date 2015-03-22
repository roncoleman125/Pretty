package pretty

import scala.io.Source
import util.FdImage
import fractop.BoxCountingMethod
import fractop.fdresult

object Analyze {
  def main(args: Array[String]): Unit = {
    val fdr = new fdresult(true)
    val boxes = Array(2, 3, 4, 6, 8, 12, 16, 32, 64, 128)

    BoxCountingMethod.setBoxOptions(boxes, false, -10, 10, 1)
    
    val lines = Source.fromFile(args(0)).getLines().toList

    val width = lines.foldLeft(0) { (width, line) =>
      val len = line.length
      if (len > width) len else width
    }
    
    val fdi = new FdImage(lines,width)

    BoxCountingMethod.BoxCountingMethod(fdr,fdi)
  }

}

