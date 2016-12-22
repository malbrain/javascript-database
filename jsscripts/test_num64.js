print("\n\nbegin test_num64.js");
print("------------------");
function primes(m) {
  var i, size = m+63>>6;
  var isPrime = new Array(size);

  for (i=0; i<size; i++) isPrime[i] = 0xffffffffffffffff;

  for (i=2; i<m; i++)
    if (isPrime[i>>6] & 1<<(i&63)) {
      for (var j=i+i; j<m; j+=i)
        isPrime[j>>6] &= ~(1<<(j&63));

	  highPrime = i;
      numPrime++;
    }
}

var test = 0xffffffffffffffff;
print(test);

var highPrime = 0, numPrime = 0;

primes(128);
print(highPrime, ":", numPrime);

var j = 4;
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
