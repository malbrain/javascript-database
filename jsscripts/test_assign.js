print("\n\nbegin test_assign.js");
print("------------------");

var a = 1, b = 2, c, d = 4;

print("Expecting 3: ", c = d = a + b);
print("Expecting 1:2:3:3: ", a,":",b,":",c,":",d);

a += 2.5;
print("Expecting 3.5: ", a);

var f = 5.5;
f += 3;

print("Expecting 8.5: ", f);
