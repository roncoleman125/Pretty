int main(int argc, char** argv) {
  char* p = "hello";
  *(p+1) = *(p+0);
}
