BEGIN {
	incomm = 0
}
/^\/\*/ && NR==1 && incomm==0 {
	incomm = 1
	#print("got cr start")
}
incomm == 0 {
	print $0
}
/\*\// && incomm == 1 {
	incomm = 0
	#print(FILENAME)
}