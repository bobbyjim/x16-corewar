

for(my $year = 1991; $year <= 2006; ++$year)
{
   for(my $month = 1; $month <= 12; ++$month)
   {
      my $file = sprintf("$year-%02d.txt", $month);
      my $url = "https://corewar.co.uk/rgc/$file";
      print "file: $file\n";
      `curl $url > $file`;
   }
}



