CFLAGS+=I+=$(PBS) -J -L -M +EA -EW +FH +STDOUT
CCSC=/cygdrive/c/Program\ Files\ \(x86\)/PICC/Ccsc.exe

%:%.c
%.o:%.c

clean: ccsc_clean 

ccsc_clean: 
	rm -f *.hex *.HEX *.ERR *.err *.COD *.cod *.d *.sym *.SYM 

install:
	icd -T$<

%.hex:%.c
	rm -f $@
	$(CCSC)  $(CFLAGS) 2>&1 $(@:.hex=.c) $(FMT_OUT)
	[ -f $(@:.hex=.cod) ] && $(PBS)/Mk/mkdepend.sh $(@:.hex=.cod) >$(@:.hex=.d) || echo .PHONY:$(@) >$(@:.hex=.d)


-include *.d

#FMT_OUT?=|awk 'BEGIN{exitcode=0;} /RAM=/{print;next;} //{ err=$$3;file=$$4;line=$$6;sub(".*:","");sub(".$$","");text=$$0;sub("l:","/cygdrive/l",file);gsub("\\\\","/",file);gsub("\"","",file);exitcode=1;printf("%s:%d: error: %s\n",file,line,text);} END{exit exitcode;}'
