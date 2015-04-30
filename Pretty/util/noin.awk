# http://stackoverflow.com/questions/9985528/how-can-i-trim-white-space-from-a-variable-in-awk
function ltrim(s) { sub(/^[ \t\r\n]+/, "", s); return s }
function rtrim(s) { sub(/[ \t\r\n]+$/, "", s); return s }
function trim(s) { return rtrim(ltrim(s)); }
BEGIN {
 srand()
}
{
lottery = rand()

if(lottery < 0.25)
  print trim($0)
else
  print $0
}
END {
# whatever
}
