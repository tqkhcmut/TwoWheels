#include <iostream>
#include <inttypes.h>

using namespace std;

typedef uint8_t BOOL;

int main(void)
{
	BOOL bTypeDef = '1';
	uint8_t i8 = '2';

	uint8_t * p_i8;
	BOOL * p_bTypeDef;
	
	cout << "bool 1: " << bTypeDef << endl;
	cout << "bool 2: " << dec << i8 << endl;
	
	p_i8 = &i8;
	p_bTypeDef = &bTypeDef;

	cout << "bool 1: " << dec << *p_bTypeDef << endl;
        cout << "bool 2: " << dec << *p_i8 << endl;
	
	p_i8 = (uint8_t *)&bTypeDef;
	
	if (p_bTypeDef == p_i8)
	{
		cout << "Equal" << endl;
	}
	else
        {
                cout << "Not equal" << endl;
        }

	p_i8 = &bTypeDef;

	if (p_bTypeDef == p_i8)
        {
                cout << "Equal" << endl;
        }
        else
        {
                cout << "Not equal" << endl;
        }
	
	return 0;
}

