/*
 * Do pass number k of n, writing "size" bytes of the given pattern "type"
 * to the file descriptor fd.   Qname, k and n are passed in only for verbose
 * progress message purposes.  If n == 0, no progress messages are printed.
 *
 * If *sizep == -1, the size is unknown, and it will be filled in as soon
 * as writing fails.
 *
 * Return 1 on write error, -1 on other error, 0 on success.
 */
static int
dopass (int fd, char const *qname, off_t *sizep, int type,
        struct randread_source *s, unsigned long int k, unsigned long int n)
{
  off_t size = *sizep;
  off_t offset;			/* Current file posiiton */
  time_t thresh IF_LINT ( = 0);	/* Time to maybe print next status update */
  time_t now = 0;		/* Current time */
  size_t lim;			/* Amount of data to try writing */
  size_t soff;			/* Offset into buffer for next write */
  ssize_t ssize;		/* Return value from write */

  /* Fill pattern buffer.  Aligning it to a 32-bit boundary speeds up randread
     in some cases.  */
  typedef uint32_t fill_pattern_buffer[3 * 1024];
  union
  {
    fill_pattern_buffer buffer;
    char c[sizeof (fill_pattern_buffer)];
    unsigned char u[sizeof (fill_pattern_buffer)];
  } r;

  off_t sizeof_r = sizeof r;
  char pass_string[PASS_NAME_SIZE];	/* Name of current pass */
  bool write_error = false;
  bool first_write = true;

  /* Printable previous offset into the file */
  char previous_offset_buf[LONGEST_HUMAN_READABLE + 1];
  char const *previous_human_offset IF_LINT ( = 0);

  if (lseek (fd, 0, SEEK_SET) == -1)
    {
      error (0, errno, _("%s: cannot rewind"), qname);
      return -1;
    }

  /* Constant fill patterns need only be set up once. */
  if (type >= 0)
    {
      lim = (0 <= size && size < sizeof_r ? size : sizeof_r);
      fillpattern (type, r.u, lim);
      passname (r.u, pass_string);
    }
  else
    {
      passname (0, pass_string);
    }

  /* Set position if first status update */
  if (n)
    {
      error (0, 0, _("%s: pass %lu/%lu (%s)..."), qname, k, n, pass_string);
      thresh = time (NULL) + VERBOSE_UPDATE;
      previous_human_offset = "";
    }

  offset = 0;
  while (true)
    {
      /* How much to write this time? */
      lim = sizeof r;
      if (0 <= size && size - offset < sizeof_r)
        {
          if (size < offset)
            break;
          lim = size - offset;
          if (!lim)
            break;
        }
      if (type < 0)
        randread (s, &r, lim);
      /* Loop to retry partial writes. */
      for (soff = 0; soff < lim; soff += ssize, first_write = false)
        {
          ssize = write (fd, r.c + soff, lim - soff);
          if (ssize <= 0)
            {
              if (size < 0 && (ssize == 0 || errno == ENOSPC))
                {
                  /* Ah, we have found the end of the file */
                  *sizep = size = offset + soff;
                  break;
                }
              else
                {
                  int errnum = errno;
                  char buf[INT_BUFSIZE_BOUND (uintmax_t)];

                  /* If the first write of the first pass for a given file
                     has just failed with EINVAL, turn off direct mode I/O
                     and try again.  This works around a bug in Linux kernel
                     2.4 whereby opening with O_DIRECT would succeed for some
                     file system types (e.g., ext3), but any attempt to
                     access a file through the resulting descriptor would
                     fail with EINVAL.  */
                  if (k == 1 && first_write && errno == EINVAL)
                    {
                      direct_mode (fd, false);
                      ssize = 0;
                      continue;
                    }
                  error (0, errnum, _("%s: error writing at offset %s"),
                         qname, umaxtostr (offset + soff, buf));

                  /* 'shred' is often used on bad media, before throwing it
                     out.  Thus, it shouldn't give up on bad blocks.  This
                     code works because lim is always a multiple of
                     SECTOR_SIZE, except at the end.  */
                  verify (sizeof r % SECTOR_SIZE == 0);
                  if (errnum == EIO && 0 <= size && (soff | SECTOR_MASK) < lim)
                    {
                      size_t soff1 = (soff | SECTOR_MASK) + 1;
                      if (lseek (fd, offset + soff1, SEEK_SET) != -1)
                        {
                          /* Arrange to skip this block. */
                          ssize = soff1 - soff;
                          write_error = true;
                          continue;
                        }
                      error (0, errno, _("%s: lseek failed"), qname);
                    }
                  return -1;
                }
            }
        }

      /* Okay, we have written "soff" bytes. */

      if (offset > OFF_T_MAX - (off_t) soff)
        {
          error (0, 0, _("%s: file too large"), qname);
          return -1;
        }

      offset += soff;

      /* Time to print progress? */
      if (n
          && ((offset == size && *previous_human_offset)
              || thresh <= (now = time (NULL))))
        {
          char offset_buf[LONGEST_HUMAN_READABLE + 1];
          char size_buf[LONGEST_HUMAN_READABLE + 1];
          int human_progress_opts = (human_autoscale | human_SI
                                     | human_base_1024 | human_B);
          char const *human_offset
            = human_readable (offset, offset_buf,
                              human_floor | human_progress_opts, 1, 1);

          if (offset == size
              || !STREQ (previous_human_offset, human_offset))
            {
              if (size < 0)
                error (0, 0, _("%s: pass %lu/%lu (%s)...%s"),
                       qname, k, n, pass_string, human_offset);
              else
                {
                  uintmax_t off = offset;
                  int percent = (size == 0
                                 ? 100
                                 : (off <= TYPE_MAXIMUM (uintmax_t) / 100
                                    ? off * 100 / size
                                    : off / (size / 100)));
                  char const *human_size
                    = human_readable (size, size_buf,
                                      human_ceiling | human_progress_opts,
                                      1, 1);
                  if (offset == size)
                    human_offset = human_size;
                  error (0, 0, _("%s: pass %lu/%lu (%s)...%s/%s %d%%"),
                         qname, k, n, pass_string, human_offset, human_size,
                         percent);
                }

              strcpy (previous_offset_buf, human_offset);
              previous_human_offset = previous_offset_buf;
              thresh = now + VERBOSE_UPDATE;

              /*
               * Force periodic syncs to keep displayed progress accurate
               * FIXME: Should these be present even if -v is not enabled,
               * to keep the buffer cache from filling with dirty pages?
               * It's a common problem with programs that do lots of writes,
               * like mkfs.
               */
              if (dosync (fd, qname) != 0)
                {
                  if (errno != EIO)
                    return -1;
                  write_error = true;
                }
            }
        }
    }

  /* Force what we just wrote to hit the media. */
  if (dosync (fd, qname) != 0)
    {
      if (errno != EIO)
        return -1;
      write_error = true;
    }

  return write_error;
}

/*
 * The passes start and end with a random pass, and the passes in between
 * are done in random order.  The idea is to deprive someone trying to
 * reverse the process of knowledge of the overwrite patterns, so they
 * have the additional step of figuring out what was done to the disk
 * before they can try to reverse or cancel it.
 *
 * First, all possible 1-bit patterns.  There are two of them.
 * Then, all possible 2-bit patterns.  There are four, but the two
 * which are also 1-bit patterns can be omitted.
 * Then, all possible 3-bit patterns.  Likewise, 8-2 = 6.
 * Then, all possible 4-bit patterns.  16-4 = 12.
 *
 * The basic passes are:
 * 1-bit: 0x000, 0xFFF
 * 2-bit: 0x555, 0xAAA
 * 3-bit: 0x249, 0x492, 0x924, 0x6DB, 0xB6D, 0xDB6 (+ 1-bit)
 *        100100100100         110110110110
 *           9   2   4            D   B   6
 * 4-bit: 0x111, 0x222, 0x333, 0x444, 0x666, 0x777,
 *        0x888, 0x999, 0xBBB, 0xCCC, 0xDDD, 0xEEE (+ 1-bit, 2-bit)
 * Adding three random passes at the beginning, middle and end
 * produces the default 25-pass structure.
 *
 * The next extension would be to 5-bit and 6-bit patterns.
 * There are 30 uncovered 5-bit patterns and 64-8-2 = 46 uncovered
 * 6-bit patterns, so they would increase the time required
 * significantly.  4-bit patterns are enough for most purposes.
 *
 * The main gotcha is that this would require a trickier encoding,
 * since lcm(2,3,4) = 12 bits is easy to fit into an int, but
 * lcm(2,3,4,5) = 60 bits is not.
 *
 * One extension that is included is to complement the first bit in each
 * 512-byte block, to alter the phase of the encoded data in the more
 * complex encodings.  This doesn't apply to MFM, so the 1-bit patterns
 * are considered part of the 3-bit ones and the 2-bit patterns are
 * considered part of the 4-bit patterns.
 *
 *
 * How does the generalization to variable numbers of passes work?
 *
 * Here's how...
 * Have an ordered list of groups of passes.  Each group is a set.
 * Take as many groups as will fit, plus a random subset of the
 * last partial group, and place them into the passes list.
 * Then shuffle the passes list into random order and use that.
 *
 * One extra detail: if we can't include a large enough fraction of the
 * last group to be interesting, then just substitute random passes.
 *
 * If you want more passes than the entire list of groups can
 * provide, just start repeating from the beginning of the list.
 */
static int const
  patterns[] =
{
  -2,				/* 2 random passes */
  2, 0x000, 0xFFF,		/* 1-bit */
  2, 0x555, 0xAAA,		/* 2-bit */
  -1,				/* 1 random pass */
  6, 0x249, 0x492, 0x6DB, 0x924, 0xB6D, 0xDB6,	/* 3-bit */
  12, 0x111, 0x222, 0x333, 0x444, 0x666, 0x777,
  0x888, 0x999, 0xBBB, 0xCCC, 0xDDD, 0xEEE,	/* 4-bit */
  -1,				/* 1 random pass */
        /* The following patterns have the frst bit per block flipped */
  8, 0x1000, 0x1249, 0x1492, 0x16DB, 0x1924, 0x1B6D, 0x1DB6, 0x1FFF,
  14, 0x1111, 0x1222, 0x1333, 0x1444, 0x1555, 0x1666, 0x1777,
  0x1888, 0x1999, 0x1AAA, 0x1BBB, 0x1CCC, 0x1DDD, 0x1EEE,
  -1,				/* 1 random pass */
  0				/* End */
};

