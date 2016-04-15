// The Great Computer Language Shootout
//  http://shootout.alioth.debian.org
//
//  Contributed by Ian Osgood
//	64 bit version by Karl Malbrain

function pad(n,width) {
  var s = n.toString();
  while (s.length < width) s = ' ' + s;
  return s;
}

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

var highPrime = 0, numPrime = 0;

primes(163840);

if (highPrime != 163819)
    print( "ERROR: expected highPrime: ", 163819, " but got ", highPrime);
else if (numPrime != 14999)
    print( "ERROR: expected numPrime: ", 14999, " but got ", numPrime);
else
	print("OK");

