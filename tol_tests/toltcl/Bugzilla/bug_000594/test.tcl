tol::console eval {
  Serie a = ConstMes;
  Serie a_ = If (1,
                 { Serie b = a;
                   MakeGlobal(b)
                 },ConstMes);
  Serie c = b;
}

tol::console stack release

if {[catch {tol::info var [list Serie b] }]} {
  set quality 1.0
  set partial "Serie b does not exist, as it should be"
} else {
  set quality 0.0
  set partial "Serie b exist, and it should not"
}
