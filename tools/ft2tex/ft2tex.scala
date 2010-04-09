#!/bin/sh
exec scala $0 $@
!#

/**
 * Script to create a texture atlas from a true type file
 * The atlas is divided in square of charSize*charSize which contains every characters
 * in "chars"
 */

import java.io._
import java.awt._
import java.awt.image.BufferedImage
import javax.imageio.ImageIO

if (args.length < 2) {
  println("Usage: ft2tex <font> <output file>");
  exit()
}

val fontFile = args(0)
val outFile = args(1)

//the character that will be available in our texture. We start at 32 cause this is the first displayable
//ascii character and add 96 to get all the letters plus most of the puncuation
var chars : String = ""
for (i <- new Range(32, 32+96, 1)) {
  chars += i.toChar
}

println("chars : " + chars)

val imgWidth : Int = 256
val imgHeight : Int = 256
//Store characters in square of charSize*charSize
val charSize : Int = 25 //256/25 = 10 and 10*10 = 100 > 96
val image = new BufferedImage(imgWidth, imgHeight, BufferedImage.TYPE_INT_ARGB)

val font = loadFont(fontFile)
println("Using font : " + font.getName())
println("\t number of glyphs : " + font.getNumGlyphs())

printToImage(chars, font, image)
saveImage(outFile, image)

def loadFont (file: String) = Font.createFont(Font.TRUETYPE_FONT, new File(file))

//Have to find bounds height that is <= charSize (since letters are higher than width, no need
//to check bounds width)
def adjustPointSize (s: String, origFont: Font, maxCharSize: Float, g: Graphics2D) = {
  var font = origFont
  var pointSize : Float = charSize
  var bounds : java.awt.geom.Rectangle2D = null
  do {
    font = font.deriveFont(pointSize)
    bounds = font.getStringBounds(s, g.getFontRenderContext())
    /*println("pointSize : %f " + pointSize)
    println("bounds height " + bounds.getHeight + ", width : " + bounds.getWidth())*/
    pointSize = pointSize - 1.0f
  } while (pointSize > 0 && bounds.getHeight > charSize);

  if (pointSize <= 0) {
    println("pointSize <= 0 ... problem")
    exit()
  }
  println("Found point size : " + pointSize)
  font
}

def printToImage (s: String, origFont: Font, image: BufferedImage) {
  val g = image.getGraphics.asInstanceOf[Graphics2D]

  val font = adjustPointSize(s, origFont, charSize, g)

  g.setFont(font)
  g.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
                     RenderingHints.VALUE_TEXT_ANTIALIAS_ON);

  val bounds = font.getStringBounds(s, g.getFontRenderContext())
  val lineMetrics = font.getLineMetrics(s, g.getFontRenderContext())
  //We are making the font monospaced, so just calculate charWidth
  val charWidth : Int = (bounds.getWidth()/s.length()).toInt
  val numCharPerLine : Int = imgWidth/charSize
  println("string bounds : (" + bounds.getHeight +"," +bounds.getWidth + ")")
  println("string length : " + s.length)
  println("char width : " + charWidth)
  println("num chars per line : " + numCharPerLine)

  //http://en.wikipedia.org/wiki/Baseline_%28typography%29
  assert(lineMetrics.getBaselineIndex == Font.ROMAN_BASELINE)
  val descent = lineMetrics.getDescent
  var ascent = lineMetrics.getAscent
  println("text descent : " + descent)
  println("text ascent : " + ascent)

  val chars : scala.List[Char] = s.toList 
  var line = 0
  var col = 0
  for (c <- chars) {
    if (line*charSize >= imgHeight) {
      println("Error : line*charSize >= imgHeight")
      exit()
    }
    //substract descent so we're sure if we have a y or p on the last line, it will be completely visible
    g.drawString(c.toString, col*charSize, (line+1)*charSize-descent)
    if (col >= numCharPerLine-1) {
      col = 0
      line = line + 1
    } else
      col = col + 1
  }

  //g.drawString(s, 50, 50)
}

def saveImage (file: String, image: BufferedImage) {
  ImageIO.write(image, "PNG", new File(file))
}

