// The Great Computer Language Shootout
//  http://shootout.alioth.debian.org
//
//  Contributed by Ian Osgood

function pad(n,width) {
  var s = n.toString();
  while (s.length < width) s = ' ' + s;
  return s;
}

function primes(m) {
  var i, size = m+31>>5;
  var isPrime = new Array(size);

  for (i=0; i<size; i++) isPrime[i] = 0xffffffff;

  for (i=2; i<m; i++)
    if (isPrime[i>>5] & 1<<(i&31)) {
      for (var j=i+i; j<m; j+=i)
        isPrime[j>>5] &= ~(1<<(j&31));

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

