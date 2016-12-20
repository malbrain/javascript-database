function primes(m) {
  var i, size = m+31>>5;
  var isPrime = new Array(size);

  for (i=0; i<size; i++) isPrime[i] = 0xffffffff;

  for (i=2; i<m; i++)
if(i==127) print(isPrime[i>>5]);
    if (isPrime[i>>5] & 1<<(i&31)) {
      for (var j=i+i; j<m; j+=i)
        isPrime[j>>5] &= ~(1<<(j&31));

	  highPrime = i;
      numPrime++;
    }
}

var highPrime = 0, numPrime = 0;

primes(128);
print(highPrime, ":", numPrime);

var j = 4;
var test = 0xffffffffffffffff;
print(test);
test &= ~(1<<(j&63));
print(test);
print(test & 1<<(j&63));

var result = new Array(5000);
var sum = 0;

var n = 4;
var i, count = 0, m = 10000<<n, size = m+31>>5;
print(size);

for (i=0; i<size; i++) result[i] = 0xffffffff;


for (var i=0; i<5000; i++) result[i] = 0xffffffff;

for (var i = 0; i < result.length; ++i)
    sum += result[i];

print(sum);
