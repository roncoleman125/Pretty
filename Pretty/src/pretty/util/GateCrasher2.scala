package pretty.util

import java.io.PrintWriter
import scala.io.Source
import java.io.File

object GateCrasher2 {
  def main(args: Array[String]): Unit = {
    if(args.length < 2) {
      println("usage: GateCrasher2 tags-file output-dir")
      System.exit(1)
    }
    
    val tags = Source.fromFile(args(0)).getLines.toList
    val outdir = args(1)
    
    tags.foreach { tag =>
      val values = tag.split(" +")
      val path = values(0)
      val errs = values(1)
      
      val ranges = values.drop(2).toList.map { n => n.toInt }
      
      if(ranges.length != 0)
        fragment(path,ranges,outdir)
    }
  }
  
  def fragment(path: String, ranges: List[Int], outdir: String) {
//    val lines = Source.fromFile(path).getLines().toList

    // Stip compiler directives
    val lines = Helper.strip(new File(path)).split("\n")
      
    val sz = ranges.length
    
    (0 until sz/2).foreach { k =>
      val target = outdir + "/" + "%02d".format(k) + "-" + Helper.basename(path)
      
      val out = new PrintWriter(target)
      
      val current = k * 2
      
      val begin = ranges(current)
      
      val end = ranges(current+1)
      
      for(i <- begin to end) {
        // Retab see https://www.kernel.org/doc/Documentation/CodingStyle
        val line = pretty.JavaHelper.retab(lines(i),8)

        out.println(line)
      }
        
      out.flush
      out.close
    }
  }
}