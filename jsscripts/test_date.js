print("\n\nbegin test_date.js");
print("------------------");
print("expect now                 : ", Date());

var date = new Date();
//date += 2000;
print("expect now + 2 seconds     : ", date +2000);

date.setTime(1000);
print("expect 1/1/1970 - tz + 1sec: ", date);
