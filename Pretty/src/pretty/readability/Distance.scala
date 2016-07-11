package pretty.readability

import org.apache.commons.lang3.StringUtils
import scala.io.Source
import java.util.Locale
import pretty.util.Helper

case class Distance(levenshtein: Int, jaro: Double, fuzzy: Int)

object Distance {

  def main(args: Array[String]): Unit = {
    test
//    val s = Source.fromFile(args(0)).mkString
//    val t = Source.fromFile(args(1)).mkString
//    val dLeven = StringUtils.getLevenshteinDistance(s, t)
//    val dJaro = StringUtils.getJaroWinklerDistance(s, t)
//    val dFuzzy = StringUtils.getFuzzyDistance(s, t, Locale.ENGLISH)
//    val sloc = s.count { p => p == '\n' }
//    val tloc = t.count { p => p == '\n' }
//    println("%s %s %d %d %d %f %d".
//        format(Helper.basename(args(0)), Helper.basename(args(1)), sloc, tloc, dLeven, dJaro, dFuzzy))
  }
  
  def getMetrics(term: String, query: String): Distance = {
    Distance(StringUtils.getLevenshteinDistance(term, query),
             StringUtils.getJaroWinklerDistance(term, query),
             StringUtils.getFuzzyDistance(term, query, Locale.ENGLISH))
  }
  
  val DIR = "tests/"  
  def test = {
    val files = List(
        "hello.c",
        "hello-kr.c",
        "hello-gnu.c",
        "hello-bsd.c",
        "hello-linux.c",
        "hello-gnu-1.c",
        "hello-gnu-2.c",
        "hello-20.c",
        "hello-40.c")
        
    val term = Source.fromFile(DIR+files(0)).mkString
    files.foreach { file =>
      val query = Source.fromFile(DIR+file).mkString
      val dLeven = StringUtils.getLevenshteinDistance(term, query)
      val dJaro = StringUtils.getJaroWinklerDistance(term, query)
      val dFuzzy = StringUtils.getFuzzyDistance(term, query, Locale.ENGLISH)
      println("%-20s %5d %5.3f %5d".format(file,dLeven,dJaro,dFuzzy))
    }
  }
}