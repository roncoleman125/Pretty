void foo() { // words = 2, sentence = 1 "{", 4+3=7
  for(int i=0; i < 10; i++)  // words = 10, sentence = 1 "for, 3+3+1+1+1+1+1+2+1+2=16
    System.out.println("i = " + i);  // words = 12, sentence = 1, 6+2+6+2+14+6+1+1=38
}

// words = 24
// length = 7+16+38 = 61
// sentence = 2