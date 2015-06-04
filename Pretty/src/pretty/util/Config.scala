package pretty.util

import scala.collection.mutable.HashMap
import scala.io.Source

object Config {

  def getKeywords(path: String): List[String] ={
    load(path,"pretty.keyword")
  }
  
  def getDelims(path: String): String = {
    val delims = load(path,"pretty.delimiters")
    
    if(delims.length == 0) "" else delims(0).replaceAll("\\s", " ")
  }
  
  def load(path: String, key: String): List[String] = {
    val keyValues = Source.fromFile(path).getLines().toList
    
    keyValues.foldLeft(List[String]()) { (values,kv) => 
      if(kv.startsWith("#"))
          values
      else if(kv.startsWith(key)) {
        val parts = kv.split(" +")
        values ++ List(parts(1))
      }
      else
        values
    }
  }
}