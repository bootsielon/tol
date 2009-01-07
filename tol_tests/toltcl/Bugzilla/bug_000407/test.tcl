tol::console eval {
  TimeSet tms1 = D(1);
  TimeSet tms2 = tms1 + D(2);
}
tol::console stack release
tol::console eval {
  TimeSet tms1 = D(1);
  TimeSet tms2 = tms1 + D(2);
}
set quality 1.0
set partial "[tol::info variable [list TimeSet tms2] ]"