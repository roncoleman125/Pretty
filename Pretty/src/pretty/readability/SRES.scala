package pretty.readability

import edu.cs.umu.sres.ParserInterface

object SRES {
  def main(args: Array[String]): Unit = {
    ParserInterface.countMetrics(args(0))
  }
}