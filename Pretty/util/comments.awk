# This program analyzes BLOCK comments in a C file and creates records of the comments
# to be added to a database.
# By Ron Coleman
# v0.1, 15-Nov-2015
BEGIN {
  FALSE = 0
  TRUE = 1
  comment = FLASE
  count = 0
  debug = TRUE
}

# Gets start of comment
/\/\*/ {
  #print "COMMENT START " NR
  comment = TRUE
  sz = 0
  start[count] = NR
}

# Collects all lines
{
  line[NR] = $0
}

# Records a comment if we're in one
comment == TRUE {
  text[count] += length($0)
}

# Gets end of comment
/\*\// {
  #print "COMMENT END " NR
  comment = FALSE
  end[count] = NR
  count++
}

# Processes the comments
END {
  n = 0
  sum = sum2 = sumText = sumText2 = 0
  for(i=0; i < count; i++) {
    # Compute the number of lines in comment
    len = end[i] - start[i] + 1

    # Compute the comment record name and write out the record
    filename = "commrecord-" i ".txt"
    for(j=start[i]; j <= end[i]; j++)
      print line[j] >> filename
      
    close(filename)
    
    sum += len
    sum2 += len * len
    sumText += text[i]
    sumText2 += text[i] * text[i]
    printf("%d %d %d %d\n",start[i],end[i],len,text[i])
  }
  u = sum / count
  var = (sum2 - sum*sum/n)/(count-1)
  stdev = sqrt(var)
  printf("number comments n: %d u: %f stdev: %f\n",count,u,stdev)
  
  u = sumText / count
  var = (sumText2 - sumText*sumText/n)/(count-1)
  stdev = sqrt(var)
  printf("comments size n: %d u: %f stdev: %f\n",count,u,stdev)
}
  
