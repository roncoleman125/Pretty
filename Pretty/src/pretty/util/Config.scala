/*
 * Copyright (c) Pretty Contributors
 * See CONTRIBUTORS.TXT for a full list of copyright holders.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Scaly Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
package pretty.util

import scala.collection.mutable.HashMap
import scala.io.Source

/**
 * This object manages access to the configuration.
 */
object Config {
  var config: Config = null
  
  def main(args: Array[String]): Unit = {
    val keywords = Config.getInstance("util/c.config").keywords
    keywords.foreach(kw => println(kw))
    println("found "+keywords.length+" keywords")
    
    val delims = Config.getInstance("util/c.config").delims
    println("delims = "+delims)
  }
  
  /** Singleton with parameter */
  def getInstance(path: String): Config = {
    if(config == null)
      config = new Config(path)
    
    config
  }
  
  /** Singleton without parameter */
  def getInstance(): Config = {
    config
  }
}

/**
 * This class contains the configuration.
 */
class Config(path: String) {
  /** Get for artefact type, literal or block */
  def isLiteral: Boolean = {
    val arts = load("pretty.artefact")
    
    arts.length == 0 || arts(0) == Constants.LITERAL
  }
  
  /** Gettor for keywords */
  def keywords: List[String] ={
    load("pretty.keyword")
  }
  
  /** Gettor for delimiters */
  def delims: String = {
    val delims = load("pretty.delimiters")
    
    val space = "\\\\s"
    if(delims.length == 0) "" else delims(0).replaceAll(space, " ")
  }
  
  /** Gettor for the measure type */
  def measure: String = {
    val measures = load("pretty.measure")
    if(measures.length == 0) Constants.DEFAULT_MEASURE else measures(0)
  }
  
  /** Loads the configuration from the path */
  def load(key: String): List[String] = {
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