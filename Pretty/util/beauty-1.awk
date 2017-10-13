# This script calculates fraction of times D < D' in data produced by run-all.ksh.
# R. Coleman
# version 0.1, 29 Sep 2017
BEGIN {
	stats["none"] = 0
}
$5 > $4 && NF == 21 {
	stats["NOI"]++
}
$6 > $4 && NF == 21 {
	stats["2"]++
}
$7 > $4 && NF == 21 {
	stats["5"]++
}
$8 > $4 && NF == 21 {
	stats["NON"]++
}
$9 > $4 && NF == 21 {
	stats["MNE"]++
}
$11 > $4 && NF == 21 {
	stats["REC"]++
}
$13 > $4 && NF == 21 {
	stats["DEC"]++
}
$15 > $4 && NF == 21 {
	stats["GNU"]++
}
$17 > $4 && NF == 21 {
	stats["K&R"]++
}
$19 > $4 && NF == 21 {
	stats["BSD"]++
}
$21 > $4 && NF == 21 {
	stats["LIN"]++
}
END {
    seq[0] = "NOI"
    seq[1] = "2"
    seq[2] = "5"
    seq[3] = "NON"
    seq[4] = "DEC"
    seq[5] = "GNU"
    seq[6] = "K&R"
    seq[7] = "BSD"
    seq[8] = "LIN"
    seq[9] = "MNE"
    seq[10] = "REC"
	print("trials:",NR)
	for(i=0; i <= 10; i++) {
	  f = stats[seq[i]] / NR * 100
	  printf("%3s: %5d %3.0f%%\n",seq[i],stats[seq[i]],f)
	}
}
