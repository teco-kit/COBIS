#undef SetVar
#define SetVar(X) this(X)/**/Set

#undef Setter
#define Setter(var)\
uint8_t SetVar(var)(char *data)\
{\
  int i;\
  if(ACLGetDataLength(data)!=sizeof(this(var))) return 0;\
  ACLAddNewType((*(data-3)),(*(data-2))+('P'-'C'));\
  rmemcpy((char*) &this(var),data,sizeof(this(var)));\
  for(i=0;i<sizeof(this(var));i++)\
    ACLAddData(data[i]);\
  return 1;\
}
