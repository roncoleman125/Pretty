#awk '/^static/{print $3}' ls.c > ~/statics.txt
#awk '{len=length($1); first=substr($0,0,1); last=substr($0,len,1); print $1,first last}'  ~/statics.txt|sort |uniq
#awk '{len=length($1); print $1,substr($0,0,len-5)}'  ~/statics.txt|sort |uniq
#awk '/^static/{print $3}' ls.c > ~/statics.txt

