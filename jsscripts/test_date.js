print("\n\nbegin test_date.js");
print("------------------");
print("expect date value                  : ", Date());

var now = Date();
now += 2000;
print("expect date value + 2 seconds      : ", now);

var date = new Date();
date.setTime(1000);
print("expect setTime 1/1/1970 - tz + 1sec: ", date);
