#include "definitions.h"


double CustomRightHandSide::value(double x, double y) const
{
  //For more elegant form please execute file "generate_rhs.py"

  double a_P = (-alpha_p * pow((x - x_p), 2) - alpha_p * pow((y - y_p), 2));

  double a_W = pow(x - x_w, 2);
  double b_W = pow(y - y_w, 2);
  double c_W = sqrt(a_W + b_W);
  double d_W = ((alpha_w * x - (alpha_w * x_w)) * (2 * x - (2 * x_w)));
  double e_W = ((alpha_w * y - (alpha_w * y_w)) * (2 * y - (2 * y_w)));
  double f_W = (pow(alpha_w * c_W - (alpha_w * r_0), 2) + 1.0);
  double g_W = (alpha_w * c_W - (alpha_w * r_0));

  return -(4 * exp(a_P) * alpha_p * (alpha_p * (x - x_p) * (x - x_p) + alpha_p * (y - y_p) * (y - y_p) - 1)
           + ((alpha_w/(c_W * f_W)) - (d_W/(2 * pow(a_W + b_W, 1.5) * f_W)) - ((alpha_w * d_W * g_W)/((a_W + b_W) * pow(f_W, 2)))
           + (alpha_w/(c_W * f_W)) - (e_W/(2 * pow(a_W + b_W, 1.5) * f_W)) - ((alpha_w * e_W * g_W)/((a_W + b_W) * pow(f_W, 2))))
           + (1.0 / epsilon) * (1.0 / epsilon) * exp(-(1 + y) / epsilon));
}

Ord CustomRightHandSide::ord(Ord x, Ord y) const 
{
  return Ord(10);
}



double CustomExactSolution::value(double x, double y) const
{ 
  double ALPHA_C = (M_PI/ omega_c);

  return exp(-alpha_p * (pow((x - x_p), 2) + pow((y - y_p), 2)))
         + (pow(sqrt(x*x + y*y), ALPHA_C) * sin(ALPHA_C * get_angle(y, x)))
         + atan(alpha_w * (sqrt(pow(x - x_w, 2) + pow(y - y_w, 2)) - r_0))
         + exp(-(1 + y) / epsilon);
};

void CustomExactSolution::derivatives (double x, double y, scalar& dx, scalar& dy) const 
{
  double a_P = -alpha_p * ( (x - x_p) * (x - x_p) + (y - y_p) * (y - y_p));

  double ALPHA_C = (M_PI/ omega_c);
  double a_C = sqrt(x*x + y*y);
  double b_C = pow(a_C, (ALPHA_C - 1.0));
  double c_C = pow(a_C, ALPHA_C);
  double d_C = ((y*y)/(x*x) + 1.0 );

  double a_W = pow(x - x_w, 2);
  double b_W = pow(y - y_w, 2);
  double c_W = sqrt(a_W + b_W);
  double d_W = (alpha_w * x - (alpha_w * x_w));
  double e_W = (alpha_w * y - (alpha_w * y_w));
  double f_W = (pow(alpha_w * c_W - (alpha_w * r_0), 2) + 1.0);

  dx = -exp(a_P) * (2 * alpha_p * (x - x_p))
       + (((ALPHA_C* x* sin(ALPHA_C * get_angle(y,x)) *b_C)/a_C)
       - ((ALPHA_C *y *cos(ALPHA_C * get_angle(y, x)) * c_C)/(pow(x, 2.0) *d_C)))
       + (d_W / (c_W * f_W));
  dy = -exp(a_P) * (2 * alpha_p * (y - y_p))
       + (((ALPHA_C* cos(ALPHA_C* get_angle(y, x)) *c_C)/(x * d_C))
       + ((ALPHA_C* y* sin(ALPHA_C* get_angle(y, x)) *b_C)/a_C))
       + (e_W / (c_W * f_W))
       + (-1) * (1.0 / epsilon) * exp(-(1 + y) / epsilon);
};

  Ord CustomExactSolution::ord(Ord x, Ord y) const 
{
  return Ord(10);
}
