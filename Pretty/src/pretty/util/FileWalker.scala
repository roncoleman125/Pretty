package pretty.util

import java.io.File

object FileWalker {
  def walk(path: String, suffix: String, action: (File) => Unit): Unit = {
    val root = new File(path)
    
    val list = root.listFiles();
    
    list.foreach { file =>
      if(file.isDirectory())
        walk(file.getAbsolutePath(), suffix, action)
        
      else if(file.toString().endsWith(suffix))
        action(file)
    }
  }

}