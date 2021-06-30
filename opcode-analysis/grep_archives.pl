
my %ops = ();
my %lengths = ();
my %files = ();

foreach my $file (<rec.games.corewar/*.txt>)
{
   open IN, $file;
   foreach (<IN>)
   {
      next if length($_) > 80; # too long
      next if split > 10;  # too long

      my $len = length $_;
      next if $len == 0;

      next unless /(DAT|MOV|ADD|SUB|MUL|DIV|MOD|JMP|JMZ|JMN|DJZ|DJN|CMP|SLT|SPL|SEQ|SNE|NOP|CNE|CLT|IJN|IJZ|XCH|FLP)\s+(.\w+),\s+(.\w+)/i;
    
      # ok we found what seems to be an opcode.  run with it.

      $files{$file}++;   
      $ops{uc $1}++;
      $len = int($len/10)*10;
      $lengths{$len}++;
   }
   close IN;
}

print join "\n", sort keys %files;
print "\n\n";

foreach (reverse sort { $ops{$a} <=> $ops{$b}} qw/DAT MOV ADD SUB MUL DIV MOD JMP JMZ JMN DJZ DJN CMP SLT SPL SEQ SNE NOP CNE CLT IJN IJZ XCH FLP/)
{
   print "$_: $ops{$_}\n";
}

#foreach (sort {$a <=> $b} keys %lengths)
#{
#   print "$_: $lengths{$_}\n";
#}

