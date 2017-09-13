* knapsack.gms

set i objects;

parameters v(i) values
           w(i) weights;

scalar C maximum weight capacity of knapsack;

equations obj objective function
          cap capacity constraint;

variable Z objective value;
binary variable x(i) object i is taken;

obj .. Z =e= sum(i, v(i) * x(i));
cap .. sum(i, w(i) * x(i)) =l= C;

model knapsack /obj, cap/;

$GDXIN knapsack_data.gdx
$LOAD i, v, w, C
$GDXIN

solve knapsack maximizing Z using MIP;