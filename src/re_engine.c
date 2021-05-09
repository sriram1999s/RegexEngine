#include<stdio.h>
#include<stdlib.h>

int match_here(char *pat, char *text, int *start , int *end);
int match_star(char ch, char* pat, char* text, int *start , int *end);
int match_plus(char ch, char* pat, char* text,int *start,int *end);
int match_qmark(char ch, char* pat, char* text,int *start,int *end);
int match_set(char* pat, char* text, int *start, int *end);
int in_set(char *trav, char *set_end,char text);
int match_spec(char ch, char* pat, char* text, int *start, int *end);
int check_macrod(char ch);
int check_macrow(char ch);
int match_w(char* pat, char* text, int *start, int *end);
int match_d(char* pat, char* text, int *start, int *end);

void match(char *pat, char *text) // primary matching function
{
	//start and end will indicate span of match in case there is
	int start=0;
	int end=-1; //since inclusive right bound
	int res=0;
	if(*pat=='\0') // in case of empty pattern
	{
		printf("%d %d %d\n",1,0,0);
		return;
	}
	if(*pat == '^')
	{
		res= match_here(pat + 1, text, &start ,&end);
	}
	else
	{
		do
		{
			if(match_here(pat, text, &start, &end))
			{
				res=1;
				break;
			}
			start++;
			end=start-1;
		} while(*text++ != '\0');
	}
	if(res==0)//no match
	{
		printf("%d\n",0);
	}
	else//match found
	{
		if(end==-1)//empty string
		end=0;
		printf("%d %d %d\n",1,start,end);
	}
}/*done*/
int match_here(char *pat, char *text, int *start, int *end)
{
	char *temp;
	// empty pattern
	if(*pat == '\0')
		return 1;
	if(pat[0]=='\\')//macros and special characters
	{
		if(pat[1]=='w')
		{
			return match_w(pat+2,text,start,end);
		}
		else if(pat[1]=='d')
		{
			return match_d(pat+2,text,start,end);
		}
		else
		{
			return match_spec(pat[1],pat+2,text,start,end);
		}
	}

	if(pat[0]== '[')//matching sets
		return match_set(pat,text,start,end);
	if(pat[1] == '*') // matching char followed by a *
		return match_star(pat[0], pat+2, text, start, end);

	if(pat[1] == '+')// matching char followed by a +
		return match_plus(pat[0], pat+2, text,start ,end);
	if(pat[1] == '?')// matching char followed by a ?
		return match_qmark(pat[0], pat+2, text, start, end);
	if(pat[0] == '$' && pat[1] == '\0')// matching end anchor
		return *text == '\0';
	if(*text != '\0' && (*pat == '.' || *pat == *text))//matching a character
	{
		(*end)++;
		return match_here(pat + 1, text + 1, start, end);
	}
	return 0;
}

int match_star(char ch, char* pat, char* text,int *start,int *end) // function that matches a non special character followed by a *
{
	int check_point;
	if(*pat=='?')//non-greedy match
	{
		++pat;
		if(*pat=='\0')//nothing follows the ?, so dont need to parse the rest of the text.
		{
			if(*end==-1)
			(*end)++;
			return 1;
		}
		do// matching from the left, sir's implementation
		{
			check_point=*end;
			if(match_here(pat, text, start ,end))
				return 1;
			*end=check_point;
			++(*end);
		} while(*text != '\0' && (*text++ == ch || ch == '.'));
	}
	else//greedy match
	{
		int temp=*start;
		char *text_check=text;
		while(*(text)!='\0' && (*text == ch || ch == '.'))//traversing till end of text as long as there is a match
		{
			text++;
			(*end)++;
		}
		while(*end>=temp-1)// after which trying to match the rest of the pattern, if no match, traversing one char backwards and checking again
		{
			check_point=*end;
			if(match_here(pat,text,start ,end))
			{
				return 1;
			}
			else
			{
				*end=check_point;
				if(text==text_check)
				break;
				--(*end);
				text--;
			}
		}
	}
	return 0;
}/*done*/

int match_plus(char ch, char* pat, char* text,int *start,int *end) // matching a char followed by a +
{
	//similar to match_star but making sure at least one char is matched
	int check_point;
	if(*pat=='?')//non-greedy match
	{
		++pat;
		if(*pat=='\0')
		{
			if(ch==*text || ( ch=='.' && *text!='\0'))
			{
				(*end)+=1;
				return 1;
			}
			else
			return 0;
		}
		while(*text != '\0' && (*text++ == ch || ch == '.'))// similar to matching a start, but using a while loop as atleast one character must be matched.
		{
			++(*end);
			check_point=*end;
			if(match_here(pat, text, start , end))
				return 1;
			*end=check_point;
		}
	}
	else// greedy match
	{
		char *text_check=text+1;
		int temp=*start;
		if(ch!='.')//tries to match atleast one character at the corresponding position if text, if no match returns 0 immediately
		{
			if(*text!=ch)
			{
				return 0;
			}
			temp+=1;
		}
		else
		{
			if(*text=='\0')
			return 0;
			text++;
			(*end)++;
			text_check=text;
			if(*text=='\0' && *pat!='\0')
			return 0;
		}

		while(*(text)!='\0' && (*text == ch || ch == '.'))// from here onwards similar to match_star
		{
			text++;
			(*end)++;
		}

		while(*end>=temp-1)
		{
			check_point=*end;
			if(match_here(pat,text, start ,end))
				return 1;
			else
			{
				*end=check_point;
				if(text==text_check)
				break;
				(*end)--;
				text--;
			}
		}
	}
	return 0;
}/*done*/
int match_qmark(char ch, char* pat, char* text,int *start,int *end)// matches a non-special character followed by a ?
{
	// first tries to match with a the character before ? and then tries to match without
	if(ch!='.')
	{
		int check_point;
		if(ch==*text)
		{
			check_point=*end;
			(*end)++;
			if(match_here(pat,text+1,start,end))// matching with
			return 1;
			else
			{
				*end=check_point;
				return match_here(pat,text,start,end); // matching without
			}
		}
		return match_here(pat,text,start,end);
	}
	else
	{
		int t_start=*start;
		int t_end=*end;
		if (match_here(pat,text+1,start,end))// matching with
		{
			//printf("YES\n");
			(*end)++;
			return 1;
		}
		else// matching without
		{
			*start=t_start;
			*end=t_end;
			return match_here(pat,text,start,end);
		}

	}
}/*done*/
int in_set(char *trav, char *set_end,char text)// takes a character and two pointers indicating start and end of set in pattern
{
	// returns a 1 if character is a part of set , else 0
	while(trav!=set_end)
	{
		if(trav[1]=='-' && trav[2]!=']') // if a range
		{
			if(((int)text<=(int)*(trav+2))&&((int)text>=(int)*trav))
			{
				return 1;

			}
			trav=trav+3; // jump over the rest of the range
		}
		else
		{
			if(text==*trav) // if just a character
			{
				return 1;
			}
			trav++;
		}
	}
	return 0;
}/*done*/
int match_set(char* pat, char* text, int *start, int *end)// matches a set followed by *,+,? or nothing
{
	char *set_end=pat;
	char *trav=pat+1;
	int checkpoint_start;
	int checkpoint_end;
	while(*set_end!=']')
	{
		set_end++;
	}
	if(*(set_end+1)=='*')// analogous to match_star
	{
		int check_point;
		pat=set_end+2;
		if(*pat=='?')
		{
			pat++;
			if(*pat=='\0')
			{	if(*end==-1)
				++(*end);
				return 1;
			}
			do
			{
				check_point=*end;
				if(match_here(pat, text, start ,end))
					return 1;
				*end=check_point;
				++(*end);
			}while(*text != '\0' && (in_set(trav,set_end,*text++)));
		}
		else
		{
			char *text_check=text;
			int temp=*start;
		
			while(*(text)!='\0' && (in_set(trav,set_end,*text)))
			{
				text++;
				(*end)++;
			}
			while(*end>=temp-1)
			{
				check_point=*end;
				if(match_here(pat,text,start ,end))
					return 1;
				else
				{
					*end=check_point;
					if(text==text_check)
					break;
					(*end)--;
					text--;
				}
			}
		}
		return 0;
	}
	else if(*(set_end+1)=='+') // analogous to match_plus
	{
		int check_point;
		pat=set_end+2;
		if(*pat=='?')
		{
			pat++;
			if(*pat=='\0')
			{
				if(in_set(trav,set_end,*text))
				{
					(*end)++;
					return 1;
				}
				else
				return 0;
			}
			while(*text != '\0' && in_set(trav,set_end,*text))
			{
				++text;
				++(*end);
				check_point=*end;
				if(match_here(pat, text, start , end))
					return 1;
				*end=check_point;
			}
		}
		else
		{
			char *text_check=text+1;
			int temp=*start;
			if(in_set(trav,set_end,*text))
			{
				++(*end);
				++text;
				text_check=text;
				temp+=1;
			}
			else
			return 0;
			
			while(*(text)!='\0' && (in_set(trav,set_end,*text)))
			{
				text++;
				(*end)++;
			}
			while(*end>=temp-1)
			{
				check_point=*end;
				if(match_here(pat,text,start ,end))
					return 1;
				else
				{
					*end=check_point;
					if(text==text_check)
					break;
					(*end)--;
					text--;
				}
			}
		}
		return 0;
	}
	else if(*(set_end+1)=='?')// analogous to match_qmark
	{
		int check_point;
		if(in_set(trav,set_end,*text))
		{
			check_point=*end;
			(*end)++;
			 if(match_here(set_end+2,text+1,start,end))
			 {
			 	return 1;
			 }
			 else
			 {
			 	*end=check_point;
			 	return match_here(set_end+2,text,start,end);
			 }
		}
		else
		{
			 //--(*end);
			 return match_here(set_end+2,text,start,end);
		}
	}
	else
	{
		if(in_set(trav,set_end,*text))// tries to match current text character with the set
		{
			text++;
			++(*end);
			return match_here(set_end+1,text,start,end);
		}
	}
	return 0;
}/*done*/
int match_spec(char ch, char* pat, char* text, int *start, int *end)// matches a special character(in the case that it was escaped) followed by *,+,? or nothing
{
	if(*pat=='*')// analogous to match_star
	{
		int check_point;
		++pat;
		if(*pat=='?')
		{
			pat++;
			if(*pat=='\0')
			{	if(*end==-1)
				++(*end);
				return 1;
			}
			do
			{
				check_point=*end;
				if(match_here(pat, text, start ,end))
					return 1;
				*end=check_point;
				++(*end);
			}while(*text != '\0' && ch==*text++);
		}
		else
		{
			char *text_check=text;
			int temp=*start;
			
			while(*(text)!='\0' && (ch==*text))
			{
				text++;
				(*end)++;
			}
			while(*end>=temp-1)
			{
				check_point=*end;
				if(match_here(pat,text,start ,end))
					return 1;
				else
				{
					*end=check_point;
					if(text==text_check)
					break;
					(*end)--;
					text--;
				}
			}
		}
		return 0;
	}
	else if(*pat=='+')// analogous to match_plus
	{
		int check_point;
		++pat;
		if(*pat=='?')
		{
			pat++;
			if(*pat=='\0')
			{
				if(ch==*text)
				{
					(*end)++;
					return 1;
				}
				else
				return 0;
			}
			while(*text != '\0' && ch==*text)
			{
				++text;
				++(*end);
				check_point=*end;
				if(match_here(pat, text, start , end))
					return 1;
				*end=check_point;
			}
		}
		else
		{
			char *text_check=text+1;
			int temp=*start;
			if(ch==*text)
			{
				++(*end);
				++text;
				text_check=text;
				temp+=1;
			}
			else
			return 0;
			
			while(*(text)!='\0' && ch==*text)
			{
				text++;
				(*end)++;
			}
			while(*end>=temp-1)
			{
				check_point=*end;
				if(match_here(pat,text,start ,end))
					return 1;
				else
				{
					*end=check_point;
					if(text==text_check)
					break;
					(*end)--;
					text--;
				}
			}
		}
		return 0;
	}

	else if(*pat=='?')// analogous to match_qmark
	{
		int check_point;
		if(ch==*text)
		{
			check_point=*end;
			(*end)++;
			 if(match_here(pat+1,text+1,start,end))
			 {
			 	return 1;
			 }
			 else
			 {
			 	*end=check_point;
			 	return match_here(pat+1,text,start,end);
			 }
		}
		else
		{
			 //--(*end);
			 return match_here(pat+1,text,start,end);
		}
	}
	else
	{
		if(ch==*text)// tries to match current text character with the spec character
		{
			text++;
			++(*end);
			return match_here(pat,text,start,end);
		}
	}
	return 0;
}/*done*/
int match_w(char* pat, char* text, int *start, int *end)
{
	if(*pat=='*')// analogous to match_star
	{
		int check_point;
		pat++;
		if(*pat=='?')
		{
			pat++;
			if(*pat=='\0')
			{	if(*end==-1)
				++(*end);
				return 1;
			}
			do
			{
				check_point=*end;
				if(match_here(pat, text, start ,end))
					return 1;
				*end=check_point;
				++(*end);
			}while(*text != '\0' && (check_macrow(*text++)));
		}
		else
		{
			char *text_check=text;
			int temp=*start;
			
			while(*(text)!='\0' && (check_macrow(*text)))
			{
				text++;
				(*end)++;
			}
			while(*end>=temp-1)
			{
				check_point=*end;
				if(match_here(pat,text,start ,end))
					return 1;
				else
				{
					*end=check_point;
					if(text==text_check)
					break;
					(*end)--;
					text--;
				}
			}
		}
		return 0;
	}
	else if(*pat=='+')// analogous to match_plus
	{
		int check_point;
		pat++;
		if(*pat=='?')
		{
			pat++;
			if(*pat=='\0')
			{
				if(check_macrow(*text))
				{
					(*end)++;
					return 1;
				}
				else
				return 0;
			}
			while(*text != '\0' && check_macrow(*text))
			{
				++text;
				++(*end);
				check_point=*end;
				if(match_here(pat, text, start , end))
					return 1;
				*end=check_point;
			}
		}
		else
		{
			char *text_check=text+1;
			int temp=*start;
			if(check_macrow(*text))
			{
				++(*end);
				++text;
				text_check=text;
				temp+=1;
			}
			else
			return 0;
			
			while(*(text)!='\0' && check_macrow(*text))
			{
				text++;
				(*end)++;
			}
			while(*end>=temp-1)
			{
				check_point=*end;
				if(match_here(pat,text,start ,end))
					return 1;
				else
				{
					*end=check_point;
					if(text==text_check)
					break;
					(*end)--;
					text--;
				}
			}
		}
		return 0;
	}
	else if(*pat=='?')// analogous to match_qmark
	{
		int check_point;
		if(check_macrow(*text))
		{
			check_point=*end;
			(*end)++;
			 if(match_here(pat+1,text+1,start,end))
			 {
			 	return 1;
			 }
			 else
			 {
			 	*end=check_point;
			 	return match_here(pat+1,text,start,end);
			 }
		}
		else
		{
			 //--(*end);
			 return match_here(pat+1,text,start,end);
		}
	}
	else
	{
		if(check_macrow(*text))// tries to match current text character with \w
		{
			++(*end);
			++text;
			return match_here(pat,text,start,end);
		}
	}
	return 0;
}
int match_d(char* pat, char* text, int *start, int *end)
{
	if(*pat=='*')// analogous to match_star
	{
		int check_point;
		pat++;
		if(*pat=='?')
		{
			pat++;
			if(*pat=='\0')
			{	if(*end==-1)
				++(*end);
				return 1;
			}
			do
			{
				check_point=*end;
				if(match_here(pat, text, start ,end))
					return 1;
				*end=check_point;
				++(*end);
			}while(*text != '\0' && (check_macrod(*text++)));
		}
		else
		{
			char *text_check=text;
			int temp=*start;
			
			while(*(text)!='\0' && (check_macrod(*text)))
			{
				text++;
				(*end)++;
			}

			while(*end>=temp-1)
			{
				check_point=*end;
				if(match_here(pat,text,start ,end))
					return 1;
				else
				{
					*end=check_point;
					if(text==text_check)
					break;
					(*end)--;
					text--;
				}
			}
		}
		return 0;
	}
	else if(*pat=='+')// analogous to match_plus
	{
		int check_point;
		pat++;
		if(*pat=='?')
		{
			pat++;
			if(*pat=='\0')
			{
				if(check_macrod(*text))
				{
					(*end)++;
					return 1;
				}
				else
				return 0;
			}
			while(*text != '\0' && check_macrod(*text))
			{
				++text;
				++(*end);
				check_point=*end;
				if(match_here(pat, text, start , end))
					return 1;
				*end=check_point;
			}
		}
		else
		{
			char *text_check=text+1;
			int temp=*start;
			if(check_macrod(*text))
			{
				++(*end);
				++text;
				text_check=text;
				temp+=1;
			}
			else
			return 0;
			
			while(*(text)!='\0' && check_macrod(*text))
			{
				text++;
				(*end)++;
			}
			while(*end>=temp-1)
			{
				check_point=*end;
				if(match_here(pat,text,start ,end))
					return 1;
				else
				{
					*end=check_point;
					if(text==text_check)
					break;
					(*end)--;
					text--;
				}
			}
		}
		return 0;
	}
	else if(*pat=='?')// analogous to match_qmark
	{
		int check_point;
		if(check_macrod(*text))
		{
			check_point=*end;
			(*end)++;
			 if(match_here(pat+1,text+1,start,end))
			 {
			 	return 1;
			 }
			 else
			 {
			 	*end=check_point;
			 	return match_here(pat+1,text,start,end);
			 }
		}
		else
		{
			 //--(*end);
			 return match_here(pat+1,text,start,end);
		}
	}
	else
	{
		if(check_macrod(*text)) // tries to match current text character with \d
		{
			++(*end);
			++text;
			return match_here(pat,text,start,end);
		}
	}
	return 0;
}
int check_macrod(char ch)// if ch is either a lowercase/uppercase alpha, a digit or an underscore , it returns 1 else 0
{
	if(((int)ch>=48)&&((int)ch<=57))
	{
		return 1;
	}
	return 0;
}
int check_macrow(char ch)// if ch is a digit it returns 1 else 0
{
	if( (((int)ch>=97)&&((int)ch<=122)) || (((int)ch>=65)&&((int)ch<=90)) || (((int)ch>=48)&&((int)ch<=57)) || ((int)ch==95))
	{
		return 1;
	}
	return 0;
}
int main()
{
	char text[5000];
	int m;
	char pat[2000];
	scanf("%[^\n]",text);
	scanf("%d\n",&m);
	int c;
	for(int i=0;i<m;i++)
	{
		scanf("%[^\n]",pat);
		match(pat,text);
		while ( (c = getchar()) != '\n' && c != EOF ); //clearing input buffer
	}
}
