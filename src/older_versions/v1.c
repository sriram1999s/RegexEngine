#include<stdio.h>
#include<stdlib.h>
#include <assert.h>

int match_here(char *pat, char *text,int *start,int *end,int flag);
int match_star(char ch, char* pat, char* text,int *start,int *end);
int match_plus(char ch, char* pat, char* text,int *start,int *end);
int match_qmark(char ch, char* pat, char* text,int *start,int *end);
void match(char *pat  , char *text)
{
	int res=0;
	int start=0;
	int end=-1;
	if(*pat =='\0')
	{
		printf("%d %d %d\n",1,0,0);
		return;
	}
	if(*pat == '^')
	{
		res= match_here(pat + 1,text , &start , &end,0);
	}
	else{
		do
		{
			if(match_here(pat, text, &start , &end,0))
			{
				res=1;
				break;
			}
			//printf("%s: %d %d\n",text,start,end);
			start++;
			end++;
		}while(*text++ != '\0');
	}
	if(res==0)
	{
		printf("%d\n",0);
	}
	else
	{
		printf("%d %d %d\n",1,start,end);
	}
	
}
int match_here(char *pat, char *text,int *start,int *end,int flag)
{
	
	// empty pattern
	if(*pat == '\0')
	{
		//++(*end);
		return 1;
	}
	if(pat[1] == '*')
		return match_star(pat[0], pat+2, text, start ,end);//*?
	if(pat[1] == '+')
		return match_plus(pat[0], pat+2, text,start ,end);
	if(pat[1]=='?')
		return match_qmark(pat[0], pat+2, text,start, end);
	
	if(pat[0] == '$' && pat[1] == '\0')
		return *text == '\0';	
	if(*text != '\0' && (*pat == '.' || *pat == *text))
	{	
		//printf("here\n");
		++(*end);
		return match_here(pat + 1, text + 1,start ,end,0);
	}	
	//--(*end);
	if(flag==0)
		*end=(*start)-1;
	return 0;	
}
int match_star(char ch, char* pat, char* text,int *start,int *end)
{
	if(*pat=='?')
	{	
		++pat;
		//printf("\t%d\n",*end);
		if(*pat=='\0')
		{
			(*end)++;
			return 1;
		}
		//--(*end);
		do
		{
		
			if(match_here(pat, text, start ,end,1))
				return 1;
			
			++(*end);
		} while(*text != '\0' && (*text++ == ch || ch == '.'));
	}
	else
	{
		if(*pat=='\0')
		{
			(*end)++;
		}
		while(*(text+1)!='\0' && (*text == ch || ch == '.'))
		{
			text++;
			(*end)++;
		}
		
		while(*end>=0)
		{
			if(match_here(pat,text,start ,end,1))
				return 1;
			else
			{
				(*end)--;
				text--;
			}
		}	
	}
	
	return 0;
}

int match_plus(char ch, char* pat, char* text,int *start,int *end)
{
	if(*pat=='?')
	{	
		++pat;
		if(*pat=='\0')
		{
			if(ch==*text || ( ch=='.' && *text!='\0'))
			{	
				(*end)+=2;
				return 1;
			}
			else
			return 0;
		}
		while(*text != '\0' && (*text++ == ch || ch == '.'))
		{
			++(*end);
			if(match_here(pat, text, start , end,1))
				return 1;	
		}
	}
	else
	{
		int temp=-1;
		if(ch!='.')
		{
			if(*text!=ch)
			{
				return 0;
			}
		}
		else 
		{
			temp=*end;
			text++;
			(*end)++;
			if(*text=='\0')
			return 0;
		}
		if(*pat=='\0')
		{
			(*end)++;
		}
		
		while(*(text+1)!='\0' && (*text == ch || ch == '.'))
		{
			text++;
			(*end)++;
		}
		
		while(*end>temp)
		{
			
			if(match_here(pat,text, start ,end,1))
				return 1;
			else
			{
				(*end)--;
				text--;
			}
		}	
	}
	return 0;
}

int match_qmark(char ch, char* pat, char* text,int *start,int *end)
{
	return 0;
}

int main()
{
	char *text2="dbbbbcjbbcde";
	char *pat2="b*?cd";
	match(pat2,text2);
}
