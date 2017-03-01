#pragma once

//	built-in property functions

typedef value_t (*propFcn)(value_t *args, value_t *thisVal, environment_t *env);
typedef value_t (*propVal)(value_t val, bool lVal);

typedef struct {
	propVal fcn;
	char *name;
	bool isBase;	// property in base, not in protoObj
	value_t str;
} PropVal;

typedef struct {
	propFcn fcn;
	char *name;
	bool isBase;	// function in base, not in protoObj
	value_t str;
} PropFcn;

