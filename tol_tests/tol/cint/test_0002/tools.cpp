////////////////////////////////////////////////////////////////////////////////
class my_inequation
////////////////////////////////////////////////////////////////////////////////
{
public:
  double a_;
  double b_;
  my_inequation(double a, double b) : a_(a), b_(b) {};
 ~my_inequation() {};
  double constraint(double x1, double x2)
  {
    double ax1b = a_*x1 + b_;
    return((pow(ax1b,3) - x2));
  }
};

