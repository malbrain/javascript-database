#pragma once
extern value_t defaultToString(value_t *args, value_t thisVal);
extern value_t fcnArrayToString(value_t *args, value_t thisVal);
extern value_t fcnObjectToString(value_t *args, value_t thisVal);
extern value_t fcnNumToString(value_t *args, value_t thisVal);
extern value_t fcnBoolToString(value_t *args, value_t thisVal);
extern value_t fcnStrToString(value_t *args, value_t thisVal);

//	built-in property functions

typedef value_t (*propFcn)(value_t *args, value_t thisVal);
typedef value_t (*propVal)(value_t val, bool lVal);


struct PropVal {
	propVal fcn;
	char *name;
	bool isBase;	// property in base, not in protoObj
};

struct PropFcn {
	propFcn fcn;
	char *name;
	bool isBase;	// function in base, not in protoObj
};

