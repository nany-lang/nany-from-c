typedef enum
{
	value1,
	value2
} e_name;

struct
{
	int a;
} myVar;

typedef struct
{
	int field1;
	char* field2;
} s_name;

typedef struct name2
{
	s_name* value;
} s_name2;

typedef struct sameName
{
	int field1;
	char* field2;
} sameName;

int main()
{
}
