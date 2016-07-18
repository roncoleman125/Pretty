void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
             program_name);
  else
    {
      printf (_("\
Usage: %s [-F DEVICE | --file=DEVICE] [SETTING]...\n\
  or:  %s [-F DEVICE | --file=DEVICE] [-a|--all]\n\
  or:  %s [-F DEVICE | --file=DEVICE] [-g|--save]\n\
"),
              program_name, program_name, program_name);
      fputs (_("\
Print or change terminal characteristics.\n\
\n\
  -a, --all          print all current settings in human-readable form\n\
  -g, --save         print all current settings in a stty-readable form\n\
  -F, --file=DEVICE  open and use the specified DEVICE instead of stdin\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      fputs (_("\
\n\
Optional - before SETTING indicates negation.  An * marks non-POSIX\n\
settings.  The underlying system defines which settings are available.\n\
"), stdout);
      fputs (_("\
\n\
Special characters:\n\
 * dsusp CHAR    CHAR will send a terminal stop signal once input flushed\n\
   eof CHAR      CHAR will send an end of file (terminate the input)\n\
   eol CHAR      CHAR will end the line\n\
"), stdout);
      fputs (_("\
 * eol2 CHAR     alternate CHAR for ending the line\n\
   erase CHAR    CHAR will erase the last character typed\n\
   intr CHAR     CHAR will send an interrupt signal\n\
   kill CHAR     CHAR will erase the current line\n\
"), stdout);
      fputs (_("\
 * lnext CHAR    CHAR will enter the next character quoted\n\
   quit CHAR     CHAR will send a quit signal\n\
 * rprnt CHAR    CHAR will redraw the current line\n\
   start CHAR    CHAR will restart the output after stopping it\n\
"), stdout);
      fputs (_("\
   stop CHAR     CHAR will stop the output\n\
   susp CHAR     CHAR will send a terminal stop signal\n\
 * swtch CHAR    CHAR will switch to a different shell layer\n\
 * werase CHAR   CHAR will erase the last word typed\n\
"), stdout);
      fputs (_("\
\n\
Special settings:\n\
   N             set the input and output speeds to N bauds\n\
 * cols N        tell the kernel that the terminal has N columns\n\
 * columns N     same as cols N\n\
"), stdout);
      fputs (_("\
   ispeed N      set the input speed to N\n\
 * line N        use line discipline N\n\
   min N         with -icanon, set N characters minimum for a completed read\n\
   ospeed N      set the output speed to N\n\
"), stdout);
      fputs (_("\
 * rows N        tell the kernel that the terminal has N rows\n\
 * size          print the number of rows and columns according to the kernel\n\
   speed         print the terminal speed\n\
   time N        with -icanon, set read timeout of N tenths of a second\n\
"), stdout);
      fputs (_("\
\n\
Control settings:\n\
   [-]clocal     disable modem control signals\n\
   [-]cread      allow input to be received\n\
 * [-]crtscts    enable RTS/CTS handshaking\n\
   csN           set character size to N bits, N in [5..8]\n\
"), stdout);
      fputs (_("\
   [-]cstopb     use two stop bits per character (one with `-')\n\
   [-]hup        send a hangup signal when the last process closes the tty\n\
   [-]hupcl      same as [-]hup\n\
   [-]parenb     generate parity bit in output and expect parity bit in input\n\
   [-]parodd     set odd parity (even with `-')\n\
"), stdout);
      fputs (_("\
\n\
Input settings:\n\
   [-]brkint     breaks cause an interrupt signal\n\
   [-]icrnl      translate carriage return to newline\n\
   [-]ignbrk     ignore break characters\n\
   [-]igncr      ignore carriage return\n\
"), stdout);
      fputs (_("\
   [-]ignpar     ignore characters with parity errors\n\
 * [-]imaxbel    beep and do not flush a full input buffer on a character\n\
   [-]inlcr      translate newline to carriage return\n\
   [-]inpck      enable input parity checking\n\
   [-]istrip     clear high (8th) bit of input characters\n\
"), stdout);
      fputs (_("\
 * [-]iutf8      assume input characters are UTF-8 encoded\n\
"), stdout);
      fputs (_("\
 * [-]iuclc      translate uppercase characters to lowercase\n\
 * [-]ixany      let any character restart output, not only start character\n\
   [-]ixoff      enable sending of start/stop characters\n\
   [-]ixon       enable XON/XOFF flow control\n\
   [-]parmrk     mark parity errors (with a 255-0-character sequence)\n\
   [-]tandem     same as [-]ixoff\n\
"), stdout);
      fputs (_("\
\n\
Output settings:\n\
 * bsN           backspace delay style, N in [0..1]\n\
 * crN           carriage return delay style, N in [0..3]\n\
 * ffN           form feed delay style, N in [0..1]\n\
 * nlN           newline delay style, N in [0..1]\n\
"), stdout);
      fputs (_("\
 * [-]ocrnl      translate carriage return to newline\n\
 * [-]ofdel      use delete characters for fill instead of null characters\n\
 * [-]ofill      use fill (padding) characters instead of timing for delays\n\
 * [-]olcuc      translate lowercase characters to uppercase\n\
 * [-]onlcr      translate newline to carriage return-newline\n\
 * [-]onlret     newline performs a carriage return\n\
"), stdout);
      fputs (_("\
 * [-]onocr      do not print carriage returns in the first column\n\
   [-]opost      postprocess output\n\
 * tabN          horizontal tab delay style, N in [0..3]\n\
 * tabs          same as tab0\n\
 * -tabs         same as tab3\n\
 * vtN           vertical tab delay style, N in [0..1]\n\
"), stdout);
      fputs (_("\
\n\
Local settings:\n\
   [-]crterase   echo erase characters as backspace-space-backspace\n\
 * crtkill       kill all line by obeying the echoprt and echoe settings\n\
 * -crtkill      kill all line by obeying the echoctl and echok settings\n\
"), stdout);
      fputs (_("\
 * [-]ctlecho    echo control characters in hat notation (`^c')\n\
   [-]echo       echo input characters\n\
 * [-]echoctl    same as [-]ctlecho\n\
   [-]echoe      same as [-]crterase\n\
   [-]echok      echo a newline after a kill character\n\
"), stdout);
      fputs (_("\
 * [-]echoke     same as [-]crtkill\n\
   [-]echonl     echo newline even if not echoing other characters\n\
 * [-]echoprt    echo erased characters backward, between `\\' and '/'\n\
   [-]icanon     enable erase, kill, werase, and rprnt special characters\n\
   [-]iexten     enable non-POSIX special characters\n\
"), stdout);
      fputs (_("\
   [-]isig       enable interrupt, quit, and suspend special characters\n\
   [-]noflsh     disable flushing after interrupt and quit special characters\n\
 * [-]prterase   same as [-]echoprt\n\
 * [-]tostop     stop background jobs that try to write to the terminal\n\
 * [-]xcase      with icanon, escape with `\\' for uppercase characters\n\
"), stdout);
      fputs (_("\
\n\
Combination settings:\n\
 * [-]LCASE      same as [-]lcase\n\
   cbreak        same as -icanon\n\
   -cbreak       same as icanon\n\
"), stdout);
      fputs (_("\
   cooked        same as brkint ignpar istrip icrnl ixon opost isig\n\
                 icanon, eof and eol characters to their default values\n\
   -cooked       same as raw\n\
   crt           same as echoe echoctl echoke\n\
"), stdout);
      fputs (_("\
   dec           same as echoe echoctl echoke -ixany intr ^c erase 0177\n\
                 kill ^u\n\
 * [-]decctlq    same as [-]ixany\n\
   ek            erase and kill characters to their default values\n\
   evenp         same as parenb -parodd cs7\n\
"), stdout);
      fputs (_("\
   -evenp        same as -parenb cs8\n\
 * [-]lcase      same as xcase iuclc olcuc\n\
   litout        same as -parenb -istrip -opost cs8\n\
   -litout       same as parenb istrip opost cs7\n\
   nl            same as -icrnl -onlcr\n\
   -nl           same as icrnl -inlcr -igncr onlcr -ocrnl -onlret\n\
"), stdout);
      fputs (_("\
   oddp          same as parenb parodd cs7\n\
   -oddp         same as -parenb cs8\n\
   [-]parity     same as [-]evenp\n\
   pass8         same as -parenb -istrip cs8\n\
   -pass8        same as parenb istrip cs7\n\
"), stdout);
      fputs (_("\
   raw           same as -ignbrk -brkint -ignpar -parmrk -inpck -istrip\n\
                 -inlcr -igncr -icrnl  -ixon  -ixoff  -iuclc  -ixany\n\
                 -imaxbel -opost -isig -icanon -xcase min 1 time 0\n\
   -raw          same as cooked\n\
"), stdout);
      fputs (_("\
   sane          same as cread -ignbrk brkint -inlcr -igncr icrnl -iutf8\n\
                 -ixoff -iuclc -ixany imaxbel opost -olcuc -ocrnl onlcr\n\
                 -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0\n\
                 isig icanon iexten echo echoe echok -echonl -noflsh\n\
                 -xcase -tostop -echoprt echoctl echoke, all special\n\
                 characters to their default values\n\
"), stdout);
      fputs (_("\
\n\
Handle the tty line connected to standard input.  Without arguments,\n\
prints baud rate, line discipline, and deviations from stty sane.  In\n\
settings, CHAR is taken literally, or coded as in ^c, 0x37, 0177 or\n\
127; special values ^- or undef used to disable special characters.\n\
"), stdout);
      emit_ancillary_info ();
    }
  exit (status);
}
