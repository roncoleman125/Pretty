package pretty

import scala.collection.mutable.HashMap
import scala.io.Source
import scala.util.Random

object Mango { 
  
}

class Mango(path: String) {
  val trans = HashMap[String,String]()
  var maps = Source.fromFile(path).getLines().toList
  
  // Load the transform pairs
  maps.foreach { line =>
    val keyValue = line.split(" ")
    trans(keyValue(0)) = keyValue(1)
  }
  
  def transform(line: String): String = {
    trans.foldLeft(line) { (newLine,kv) =>
      newLine.replaceAll(kv._1,kv._2)
    }
  }
  
  def none(path: String): List[String] = {
    Source.fromFile(path).getLines().toList
  }
  
  def nonmnemonic(path: String): List[String] = {
    val lines = Source.fromFile(path).getLines().toList
    lines.map { line => transform(line) }
  }
  
  def noIndent(path: String): List[String] = {
    val lines = Source.fromFile(path).getLines().toList
    lines.map { line =>
      line.trim
    }
  }

  def randomIndent(path: String, n: Int): List[String] = {
    val ran = new Random(0)
    val lines = Source.fromFile(path).getLines().toList
    lines.map { line =>
      val r = ran.nextInt(n)
      val indent = (1 to r).foldLeft("") { (xs, x) => xs + " " }
      indent + line
    }
  }
}