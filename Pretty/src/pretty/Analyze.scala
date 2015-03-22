package pretty

import scala.io.Source
import util.FdImage
import fractop.BoxCountingMethod
import fractop.fdresult

object Analyze {
  def main(args: Array[String]): Unit = {
    // Box value were taken from the GUI implementation.
    val boxes = Array(2, 3, 4, 6, 8, 12, 16, 32, 64, 128)

    // Parameters, -10 (start q), 10 (end q), and 1 (increment) used only
    // for multifractal analysis which is false
    BoxCountingMethod.setBoxOptions(boxes, false, -10, 10, 1)
    
    val fdi = FdImage.getInstance(args(0))
    
    val fdr = new fdresult(true)
    
    BoxCountingMethod.BoxCountingMethod(fdr,fdi)
    
    val slope = fdr.CalcLogSlope
    
    println("FD = "+slope)
  }

}

