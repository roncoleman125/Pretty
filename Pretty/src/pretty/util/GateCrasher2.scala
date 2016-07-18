package pretty.util

import java.io.PrintWriter

import scala.io.Source

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
      
      val lineNums = values.drop(1).toList.map { n => n.toInt }
      
      if(lineNums.length != 0)
        fragment(path,lineNums,outdir)
    }
  }
  
  def fragment(path: String, lns: List[Int], outdir: String) {
    val lines = Source.fromFile(path).getLines().toList
    val lineNums = lns ++ List(lines.length)
      
    (0 until lineNums.length-1).foreach { k =>
      val target = outdir + "/" + "%02d".format(k) + "-" + Helper.basename(path)
      
      val out = new PrintWriter(target)
      
      val begin = lineNums(k)
      
      val end = lineNums(k+1) - 1
      
      for(i <- begin to end) {
        val line = pretty.JavaHelper.retab(lines(i),8)
        out.println(line)
      }
        
      out.flush
      out.close
    }
  }
}