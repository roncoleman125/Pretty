package pretty.util

object Helper {
  val state = System.getProperty("debug")
  
  val DEBUG = if(state == null || state != "true") false else true
  
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