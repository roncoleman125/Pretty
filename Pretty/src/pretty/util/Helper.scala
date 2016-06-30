package pretty.util

object Helper {
  val DEBUG = if(System.getProperty("debug") != null) true else false
  
  def logger(s: String): Unit = {
    if(DEBUG)
      println(s)
  }
  
  def basename(s: String): String = {
    val j = s.lastIndexOf('/')
    if(j == -1)
      s
    else
      s.substring(j+1)
  }
  
  def decomment(s: String): String = {
    null
  }
}