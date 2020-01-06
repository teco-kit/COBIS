//new rpc



void EscapeAndPacket(char* in, char* out, int inlen,int* outlen)
{
	int i,pos;
	
	pos=0;
	out[pos++]=0;
	for (i=0;i<inlen;i++)
	{
		if (in[i]==0)
		{
			out[pos++]=219;
			out[pos++]=1;
		}
		else if (in[i]==192)
		{
			out[pos++]=219;
			out[pos++]=193;
		}
		else if (in[i]==219)
		{
			out[pos++]=219;
			out[pos++]=220;
		}
		else
		{
			out[pos++]=in[i];
		}
	}
	
	out[pos++]=192;
	*outlen=pos;

}

boolean DeEscapePacket(char* in, char* out, int inlen,int* outlen)
{
	int i,pos;
	
	
	
	if (in[0]!=0) return false;
	if (in[inlen-1]!=192) return false;

	pos=0;
	i=1;
	while (i<inlen-1)
	{
		if (in[i]==0) return false;		//not allowed
		if (in[i]==192) return false;
		
		if (in[i]==219)
		{
			i++;
			out[pos++]=in[i++]-1;
		}
		else 
		{
			out[pos++]=in[i++];
		}
	}
	*outlen=pos;
}
