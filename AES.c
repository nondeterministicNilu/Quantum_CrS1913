#include<stdio.h>
#include<stdlib.h>

int sbox[16][16];

char *str, *key;

int rnd, rndCnstnt[10] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

int state[4][4], Key[4][4];

int mulCol[4][4], hello=0x11B;

int fieldInv(int a)
{
	int j,i,b=0,mod=283;
	for (i=1;i<256;i++)
	{
		b=0;
		for(j=0;j<8;j++)
		{
			int temp=(a>>j)&1;
			if(temp==1)
				b=b^(i<<j);	
		}
		int h=(b>>8);
		while(h>0)
		{
			int l=8,temp=(b>>l)&1;
			while(temp!=1)
			{
				l++;
				temp=(b>>l)&1;
			}
			int temp1=(mod<<(l-8));
			b=b^temp1;
			h=(b>>8);
		}
		if (b==1)
			return i;
	}
	return -1;
}

int inv(int k)
{
	int c=0x63,i;
	if (k==0)
		return c;
	int a;
	a=fieldInv(k);
	int b=0;
	for (i=0;i<8;i++)
	{
		int b1,b2,b3,b4,b5,b6,b7;
		b1=(a>>i)&1;
		b4=(a>>((i+4)%8))&1;
		b5=(a>>((i+5)%8))&1;
		b6=(a>>((i+6)%8))&1;
		b7=(a>>((i+7)%8))&1;
		b2=(c>>i)&1;
		b+=((b1+b4+b5+b6+b7+b2)%2)<<i;
	}
	return b;
}

void addRoundKey()
{
	int i,j,k,l,i1,j1,k1;
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			state[i][j]=state[i][j]^Key[i][j];
	}
	int temp[4][4],tmp[4];
	for(j=3;j>=0;j--)
	{
		if(j==3)
		{
			for(k=0;k<4;k++)
				tmp[k]=Key[(k+1)%4][j];
			for(k=0;k<4;k++)
			{
					l=tmp[k];
					i1=(l>>4);
					k1=(i1<<4);
					j1=l^k1;
					temp[k][j]=sbox[i1][j1];
					if(k==0)
						temp[k][j]=temp[k][j]^rndCnstnt[rnd];
			}
			for (i=0;i<4;i++)
				temp[i][0]=temp[i][j]^Key[i][0];
		}
		else
		{
			for (i=0;i<4;i++)
				temp[i][3-j]=temp[i][2-j]^Key[i][3-j];
		}
	}
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			Key[i][j]=temp[i][j];
	}
	rnd++;
}

void subBytes()
{
	int i,j,k,i1,j1,k1;
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			k=state[i][j];
			i1=(k>>4);
			k1=(i1<<4);
			j1=k^k1;
			state[i][j]=sbox[i1][j1];
		}
	}
}

void shiftRow()
{
	int i,j,temp[4];
	for (i=0;i<4;i++)
	{
		if (i>0)
		{
			for (j=0;j<4;j++)
				temp[j]=state[i][(j+i)%4];
			for (j=0;j<4;j++)
				state[i][j]=temp[j];	
		}
	}
}

void mixColumns()
{
	int i,j,k,n,t,temp[4][4];
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			temp[i][j]=0;
			for(k=0;k<4;k++)
			{
				if(mulCol[i][k]==0x1)
					temp[i][j]^=state[k][j];
				else if(mulCol[i][k]==0x2)
				{
					n=state[k][j]<<1;
					t=n>>8;
					if (t>0)
						n=n^hello;
					temp[i][j]^=n;
				}
				else
				{
					n=state[k][j]<<1;
					t=n>>8;
					if (t>0)
						n=n^hello;
					n=n^state[k][j];
					temp[i][j]^=n;
				}
			}
		}
	}
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			state[i][j]=temp[i][j];
	}
}

void sBox()
{
	char i,j;
	int k,c=0x63;
	for (i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
		{
			k=i;
			k=(k<<4);
			k=k+j;
			sbox[i][j]=inv(k);
		}
	}
}

int main()
{
	key="Niladri Banerjee";
	str="Hi, I am Niladri";
	printf("Input string (PlainText) of 16 length string: %s\n",str);
//scanf("%s",str); 
	printf("Key of 16 length string: %s\n",key);
//scanf("%s",key); 
	int i,j,k;
	sBox();
	rnd=0;
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(j==i)
				mulCol[i][j]=0x2;
			else if((j-i)==1 || (j-i)==-3)
				mulCol[i][j]=0x3;
			else
				mulCol[i][j]=0x1;
		}
	}
	k=0;
	for(j=0;j<4;j++)
	{
		for (i=0;i<4;i++)
		{
			Key[i][j]=key[k];
			state[i][j]=str[k];
			k++;
		}
	}
	addRoundKey();
	for(i=1;i<11;i++)
	{
		subBytes();
		shiftRow();
		if (i<10)
			mixColumns();
		addRoundKey();
	}
	printf("\nHence the ciphertext is: ");
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			printf(" 0x%0.2x ",state[j][i]);
	}
	printf("\n");
	for (i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			printf(" %c ",state[j][i]);
	}
	return 0;
}
