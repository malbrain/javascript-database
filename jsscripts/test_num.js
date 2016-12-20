print((10240<<4) + 31 >> 5);
print((10240<<4) + 63 >> 6);

var j = 4;
var test = 0xffffffffffffffff;
print("all binary ones: ", test);
test &= ~(1<<(j&63));
print("mask with -1: ", test);
print("mask with  1: ", test & 1<<(j&63));

var result = new Array(5000);
var sum = 0;

var n = 4;
var i, count = 0, m = 10000<<n, size = m+31>>5;
print(size);

for (i=0; i<size; i++) result[i] = 0xffffffff;


for (i=0; i<5000; i++) result[i] = 0xffffffff;

for (i = 0; i < result.length; ++i)
    sum += result[i];

print(sum);
